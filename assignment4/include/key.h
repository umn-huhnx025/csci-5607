#pragma once

#include "glm/glm.hpp"

#include "door.h"

class Key {
 public:
  float x, y;
  Door* door;
  int index;
  glm::vec3 color;

  Key(float x_ = 0, float y_ = 0);
};
