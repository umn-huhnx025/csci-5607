#include "bounding_box.h"

#include <algorithm>
#include <cstdio>

BoundingBox::BoundingBox(double x_min_, double x_max_, double y_min_,
                         double y_max_, double z_min_, double z_max_)
    : x_min(x_min_),
      x_max(x_max_),
      y_min(y_min_),
      y_max(y_max_),
      z_min(z_min_),
      z_max(z_max_) {
  c.x = (x_min + x_max) / 2.;
  c.y = (y_min + y_max) / 2.;
  c.z = (z_min + z_max) / 2.;
}

BoundingBox::BoundingBox(Shape *s) {
  std::vector<Vector> bounds = s->GetExtents();
  x_min = bounds[0].x;
  y_min = bounds[0].y;
  z_min = bounds[0].z;
  x_max = bounds[1].x;
  y_max = bounds[1].y;
  z_max = bounds[1].z;
  c.x = (x_min + x_max) / 2.;
  c.y = (y_min + y_max) / 2.;
  c.z = (z_min + z_max) / 2.;
}

Vector BoundingBox::GetNormal(Vector p, Vector v) { return Vector(); }

hit_t BoundingBox::GetRayIntersection(Ray r) {
  // http://www.cs.utah.edu/~awilliam/box/box.pdf

  // printf("BoundingBox:\n");
  // printf("%f %f %f %f %f %f\n", x_min, x_max, y_min, y_max, z_min, z_max);
  // printf("Ray from %f %f %f going to %f %f %f\n", r.p.x, r.p.y, r.p.z, r.v.x,
  // r.v.y, r.v.z);
  hit_t hit;
  hit.s = this;
  hit.r = r;
  hit.hit = false;
  hit.t = Ray::TMAX;

  double divx = 1 / r.v.x;
  double t_min = (x_min - r.p.x) * divx;
  double t_max = (x_max - r.p.x) * divx;
  // printf("r.v.x = %f, < 0? %d\n", r.v.x, r.v.x < 0);
  if (r.v.x <= 0) std::swap(t_min, t_max);

  double divy = 1 / r.v.y;
  double t_ymin = (y_min - r.p.y) * divy;
  double t_ymax = (y_max - r.p.y) * divy;
  if (r.v.y <= 0) std::swap(t_ymin, t_ymax);

  if (t_min > t_ymax || t_ymin > t_max) {
    // printf("tmin = %f, tmax = %f\n", t_min, t_max);
    // printf("Ray missed 1\n");
    return hit;
  }

  double divz = 1 / r.v.z;
  double t_zmin = (z_min - r.p.z) * divz;
  double t_zmax = (z_max - r.p.z) * divz;
  if (r.v.z <= 0) std::swap(t_zmin, t_zmax);

  if (t_min > t_zmax || t_zmin > t_max) {
    // printf("Ray missed 2\n");
    return hit;
  }

  if (t_ymin > t_min) t_min = t_ymin;
  if (t_ymax < t_max) t_max = t_ymax;

  if (t_zmin > t_min) t_min = t_zmin;
  if (t_zmax < t_max) t_max = t_zmax;

  if (!((t_min < Ray::TMAX) && (t_max > Ray::TMIN))) {
    // printf("Ray missed 3\n");
    return hit;
  }

  // printf("Ray hit\n");
  hit.hit = true;
  return hit;
}

std::vector<Vector> BoundingBox::GetExtents() {
  std::vector<Vector> ret;
  Vector min(x_min, y_min, z_min);
  Vector max(x_max, y_max, z_max);
  ret.push_back(min);
  ret.push_back(max);
  return ret;
}

BoundingBox *BoundingBox::Combine(const BoundingBox &b1,
                                  const BoundingBox &b2) {
  return new BoundingBox(fmin(b1.x_min, b2.x_min), fmax(b1.x_max, b2.x_max),
                         fmin(b1.y_min, b2.y_min), fmax(b1.y_max, b2.y_max),
                         fmin(b1.z_min, b2.z_min), fmax(b1.z_max, b2.z_max));
}
