#pragma once

#include <vector>

#include "bounding_box.h"
#include "shape.h"

enum Axis {
  AXIS_X,
  AXIS_Y,
  AXIS_Z,
  N_AXES,
};

class BVHNode : public Shape {
 public:
  BoundingBox *box;
  Shape *left;
  Shape *right;

  BVHNode(std::vector<Shape *> shapes, Axis axis);

  Vector GetNormal(Vector v, Vector p);
  hit_t GetRayIntersection(Ray r);
  std::vector<Vector> GetExtents();
};
