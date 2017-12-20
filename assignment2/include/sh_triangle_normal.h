#pragma once

#include "sh_triangle.h"

class NormalTriangle : public Triangle {
 public:
  Vector n1, n2, n3;

  NormalTriangle(Vector v1_, Vector v2_, Vector v3_, Vector n1_, Vector n2_,
                 Vector n3_, material_t mat_);

  Vector GetNormal(Vector v, Vector p);
};
