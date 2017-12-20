#include "map.h"

#include "glm/glm.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>

Map::Map(int width_, int height_)
    : width(width_ + 2),
      height(height_ + 2),
      start_x(0),
      start_y(0),
      goal_x(width_ - 1),
      goal_y(height_ - 1),
      walls(std::vector<Wall *>()),
      keys(std::vector<Key *>(MAX_DOORS)),
      currentDoorColor(glm::vec3(1, 0, 0)),
      heldKey(nullptr),
      gameOver(false) {}

Map::Map(const char *filename)
    : currentDoorColor(glm::vec3(1, 0, 0)), heldKey(nullptr), gameOver(false) {
  std::ifstream f(filename);
  f >> width >> height;
  width += 2;
  height += 2;

  keys = std::vector<Key *>(MAX_DOORS);
  std::vector<Door *> doors(MAX_DOORS);

  char c;
  f.get(c);
  int map_x, map_y;
  for (int i = 0; i < width; i++) {
    walls.push_back(new Wall(i, 0));
    walls.push_back(new Wall(i, height - 1));
  }
  for (int i = 1; i < height - 1; i++) {
    walls.push_back(new Wall(0, i));
    walls.push_back(new Wall(width - 1, i));
  }
  for (int i = 1; i < height - 1; i++) {
    map_y = height - 1 - i;
    for (int j = 1; j < width - 1 && f.get(c); j++) {
      if (c == '\n') {
        j--;
        continue;
      }
      map_x = j;
      // printf("Read character %#04x\n", c);
      // printf("Map x, y = %d, %d\n", map_x, map_y);
      if (c == 'W') {
        walls.push_back(new Wall(map_x, map_y));
        // printf("Made wall at %d, %d\n", map_x, map_y);
      } else if (c == 'S') {
        start_x = map_x;
        start_y = map_y;
        // printf("Set start to %d, %d\n", map_x, map_y);
      } else if (c == 'G') {
        goal_x = map_x;
        goal_y = map_y;
        // printf("Set goal to %d, %d\n", map_x, map_y);
      } else if (c >= 'A' && c <= 'E') {
        doors[c - 'A'] = new Door(map_x, map_y);
        // printf("Added door %c (%p) at %d, %d\n", 'A' + c - 'A', doors[c -
        // 'A'],
        //        map_x, map_y);
      } else if (c >= 'a' && c <= 'e') {
        keys[c - 'a'] = new Key(map_x, map_y);
        // printf("Added key %c at %d, %d\n", 'a' + c - 'a', map_x, map_y);
      }
    }
  }
  // printf("Made new map with %d walls\n", walls.size());

  for (int i = 0; i < MAX_DOORS; i++) {
    if (doors[i] && keys[i]) {
      // doors[i]->key = keys[i];
      keys[i]->door = doors[i];
      keys[i]->color = currentDoorColor;
      keys[i]->index = i;
    } else if (doors[i] || keys[i]) {
      fprintf(
          stderr,
          "Error parsing map file: check there is one key for every door.\n");
      exit(1);
    }
    getNextDoorColor();
  }
}

bool Map::WillCollide(float x, float y) {
  float w1x, w2x, w1y, w2y;
  float threshold = 0.05;
  for (auto w : walls) {
    // Are we running into a wall?

    // printf("Checking wall at %g %g\n", w->x, w->y);
    w1x = (w->x - 0.5 * std::sqrt(2) - threshold) * scale;
    w1y = (w->y - 0.5 * std::sqrt(2) - threshold) * scale;
    w2x = (w->x + 0.5 * std::sqrt(2) + threshold) * scale;
    w2y = (w->y + 0.5 * std::sqrt(2) + threshold) * scale;
    // printf("x, y = %f %f\n", x, y);
    // printf("w1x = %f, w2x = %f, w1y = %f, w2y = %f\n", w1x, w2x, w1y, w2y);

    if (x < w2x && x > w1x && y > w1y && y < w2y) {
      // printf("Ran into wall\n");
      return true;
    }
  }
  if (!heldKey) {
    // Can we pick up a key?
    for (auto k : keys) {
      if (k && !k->door->open) {
        w1x = (k->x - 0.5) * scale;
        w1y = (k->y - 0.5) * scale;
        w2x = (k->x + 0.5) * scale;
        w2y = (k->y + 0.5) * scale;

        if (x < w2x && x > w1x && y > w1y && y < w2y) {
          heldKey = k;
          // printf("Picked up key\n");
        }
      }
    }
  } else {
    // printf("Checking door %p\n", heldKey->door);

    // Can we open the door with the key we have?
    w1x = (heldKey->door->x - 0.5 * std::sqrt(2) - threshold) * scale;
    w1y = (heldKey->door->y - 0.5 * std::sqrt(2) - threshold) * scale;
    w2x = (heldKey->door->x + 0.5 * std::sqrt(2) + threshold) * scale;
    w2y = (heldKey->door->y + 0.5 * std::sqrt(2) + threshold) * scale;

    if (x < w2x && x > w1x && y > w1y && y < w2y && !heldKey->door->open) {
      // printf("Opened door %p\n", heldKey->door);
      heldKey->door->open = true;

      // Drop the key
      heldKey = nullptr;
    }
  }
  for (auto k : keys) {
    // Are we running into a closed door we don't have the key for?
    if (k) {
      w1x = (k->door->x - 0.5 * std::sqrt(2) - threshold) * scale;
      w1y = (k->door->y - 0.5 * std::sqrt(2) - threshold) * scale;
      w2x = (k->door->x + 0.5 * std::sqrt(2) + threshold) * scale;
      w2y = (k->door->y + 0.5 * std::sqrt(2) + threshold) * scale;

      if (x < w2x && x > w1x && y > w1y && y < w2y && !k->door->open) {
        // printf("Ran into closed door\n");
        return true;
      }
    }
  }

  // Did we win?
  w1x = (goal_x - 0.5) * scale;
  w1y = (goal_y - 0.5) * scale;
  w2x = (goal_x + 0.5) * scale;
  w2y = (goal_y + 0.5) * scale;

  if (x < w2x && x > w1x && y > w1y && y < w2y) gameOver = true;

  return false;
}

void Map::getNextDoorColor() {
  if (currentDoorColor == glm::vec3(1, 0, 0)) {
    currentDoorColor = glm::vec3(1, 0.5, 0);
  } else if (currentDoorColor == glm::vec3(1, 0.5, 0)) {
    currentDoorColor = glm::vec3(1, 1, 0);
  } else if (currentDoorColor == glm::vec3(1, 1, 0)) {
    currentDoorColor = glm::vec3(0, 0, 1);
  } else if (currentDoorColor == glm::vec3(0, 0, 1)) {
    currentDoorColor = glm::vec3(1, 0, 1);
  } else if (currentDoorColor == glm::vec3(1, 0, 1)) {
    currentDoorColor = glm::vec3(1, 0, 0);
  } else {
    fprintf(stderr, "Unexpected color %g %g %g\n", currentDoorColor.x,
            currentDoorColor.y, currentDoorColor.z);
  }
}
