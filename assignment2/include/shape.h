#pragma once

#include "ray.h"

#include <cmath>
#include <vector>

class Shape;

typedef struct {
  double r, g, b;
  operator bool();
} color_t;

typedef struct {
  bool hit;
  Shape *s;
  Ray r;
  double t;
} hit_t;

color_t operator+(const color_t &a, const color_t &b);
color_t operator-(const color_t &a, const color_t &b);
color_t operator*(const color_t &a, double s);
color_t operator*(const color_t &a, const color_t &b);
color_t operator/(const color_t &a, double s);

typedef struct {
  color_t a, d, s;
  double ns;
  color_t t;
  double ior;
  color_t e;
} material_t;

class Shape {
 public:
  Vector c;
  material_t mat;
  Vector blur;

  virtual hit_t GetRayIntersection(Ray r) = 0;
  virtual Vector GetNormal(Vector v, Vector p) = 0;
  virtual std::vector<Vector> GetExtents() = 0;
};
