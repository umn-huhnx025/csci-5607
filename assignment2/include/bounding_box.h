#pragma once

#include "shape.h"

class BoundingBox : public Shape {
 public:
  double x_min, x_max, y_min, y_max, z_min, z_max;

  BoundingBox(double x_min_, double x_max_, double y_min_, double y_max_,
              double z_min_, double z_max_);
  BoundingBox(Shape *s);

  Vector GetNormal(Vector v, Vector p);
  hit_t GetRayIntersection(Ray r);
  std::vector<Vector> GetExtents();

  static BoundingBox *Combine(const BoundingBox &b1, const BoundingBox &b2);
};
