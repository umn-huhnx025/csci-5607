#include "sh_triangle.h"

#include <cstdio>

Triangle::Triangle() : v1(Vector()), v2(Vector()), v3(Vector()) {
  c = Vector();
}

Triangle::Triangle(Vector v1_, Vector v2_, Vector v3_, material_t mat_)
    : v1(v1_), v2(v2_), v3(v3_) {
  mat = mat_;
  c = (v1 + v2 + v3) / 3.;
}

Vector Triangle::GetNormal(Vector v, Vector p) {
  Vector cp1 = Vector::Cross(v2 - v1, v3 - v1);
  Vector cp2 = Vector::Cross(v3 - v1, v2 - v1);

  //   printf("v: %f %f %f\n", v.x, v.y, v.z);
  //   printf("cp1: %f %f %f\n", cp1.x, cp1.y, cp1.z);
  //   printf("cp2: %f %f %f\n", cp2.x, cp2.y, cp2.z);
  //   printf("v dot cp1: %f\n", Vector::Dot(v, cp1));
  //   printf("v dot cp2: %f\n", Vector::Dot(v, cp2));

  return Vector::Dot(v, cp1) > 0 ? cp1 : cp2;
}

hit_t Triangle::GetRayIntersection(Ray r) {
  hit_t hit;
  hit.t = Ray::TMAX;
  hit.s = this;
  hit.r = r;
  hit.hit = false;

  Vector v[3];

  v[0] = v1 - v2;
  v[1] = v1 - v3;
  v[2] = r.v;
  double det_a = Vector::Determinant(v);

  v[0] = v1 - v2;
  v[1] = v1 - v3;
  v[2] = v1 - r.p;
  double t = Vector::Determinant(v) / det_a;

  if (t < Ray::TMIN || t > Ray::TMAX) {
    return hit;
  }

  v[0] = v1 - v2;
  v[1] = v1 - r.p;
  v[2] = r.v;
  double c = Vector::Determinant(v) / det_a;

  if (c < 0 || c > 1) {
    return hit;
  }

  v[0] = v1 - r.p;
  v[1] = v1 - v3;
  v[2] = r.v;
  double b = Vector::Determinant(v) / det_a;

  if (b < 0 || b > 1 - c) {
    return hit;
  }

  hit.t = t;
  hit.hit = true;
  return hit;
}

std::vector<Vector> Triangle::GetExtents() {
  std::vector<Vector> ret;
  Vector min =
      Vector(fmin(v1.x, fmin(v2.x, v3.x)), fmin(v1.y, fmin(v2.y, v3.y)),
             fmin(v1.z, fmin(v2.z, v3.z)));
  Vector max =
      Vector(fmax(v1.x, fmax(v2.x, v3.x)), fmax(v1.y, fmax(v2.y, v3.y)),
             fmax(v1.z, fmax(v2.z, v3.z)));
  ret.push_back(min);
  ret.push_back(max);
  return ret;
}
