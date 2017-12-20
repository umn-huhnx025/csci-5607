#pragma once

#include "shape.h"

class Sphere : public Shape {
 public:
  double r;

  Sphere();
  Sphere(Vector c_, double r_, material_t mat_);
  Sphere(Vector c_, double r_, material_t mat_, Vector blur_);

  Vector GetNormal(Vector v, Vector p);
  hit_t GetRayIntersection(Ray r);
  std::vector<Vector> GetExtents();
};
