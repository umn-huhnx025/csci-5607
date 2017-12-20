#pragma once

#include "vector.h"

class Ray {
 public:
  Vector p;
  Vector v;
  double t;

  static constexpr double TMIN = 0.01;
  static constexpr double TMAX = 100000;

  Ray();
  Ray(Vector p_, Vector v_);
  Ray(Vector p_, Vector v_, double t);

  Vector FindPoint(double t);
  void Normalize();
};
