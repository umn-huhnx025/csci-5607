#include "sh_triangle_normal.h"

NormalTriangle::NormalTriangle(Vector v1_, Vector v2_, Vector v3_, Vector n1_,
                               Vector n2_, Vector n3_, material_t mat_)
    : n1(n1_), n2(n2_), n3(n3_) {
  v1 = v1_;
  v2 = v2_;
  v3 = v3_;
  c = (v1 + v2 + v3) / 3.;
  mat = mat_;
}

Vector NormalTriangle::GetNormal(Vector v, Vector p) {
  Vector temp[3];

  temp[0] = v1 - v2;
  temp[1] = v1 - v3;
  temp[2] = v;
  double det_a = Vector::Determinant(temp);

  temp[0] = v1 - v2;
  temp[1] = v1 - p;
  temp[2] = v;
  double c = Vector::Determinant(temp) / det_a;

  temp[0] = v1 - p;
  temp[1] = v1 - v3;
  temp[2] = v;
  double b = Vector::Determinant(temp) / det_a;
  double a = 1 - b - c;

  return (n1 * a + n2 * b + n3 * c).Normalized();
}
