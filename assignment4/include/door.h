#pragma once

class Door {
 public:
  float x, y;
  bool open;

  Door(float x_ = 0, float y_ = 0);

  static void getNextDoorColor();
};
