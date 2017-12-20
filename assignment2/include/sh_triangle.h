#pragma once

#include "shape.h"

class Triangle : public Shape {
 public:
  Vector v1, v2, v3;
  Triangle();
  Triangle(Vector v1_, Vector v2_, Vector v3_, material_t mat_);

  double Determinant(Vector v[3]);

  virtual Vector GetNormal(Vector v, Vector p);
  hit_t GetRayIntersection(Ray r);
  std::vector<Vector> GetExtents();
};
