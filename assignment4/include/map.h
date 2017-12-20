#pragma once

#include <vector>

#include "glm/glm.hpp"

// #include "door.h"
#include "key.h"
#include "wall.h"

class Map {
 public:
  int width, height;
  float start_x, start_y, goal_x, goal_y;
  std::vector<Wall*> walls;
  // std::vector<Door*> doors;
  std::vector<Key*> keys;
  glm::vec3 currentDoorColor;
  Key* heldKey;
  bool gameOver;

  const int scale = 1;

  static const int MAX_DOORS = 5;

  Map(int width_ = 0, int height_ = 0);
  Map(const char* filename);

  bool WillCollide(float x, float y);
  void getNextDoorColor();
};
