#include "sh_sphere.h"

#include <cstdio>

Sphere::Sphere() : r(1) { c = Vector(); }

Sphere::Sphere(Vector c_, double r_, material_t mat_) : r(r_) {
  mat = mat_;
  c = c_;
  blur = Vector();
}

Sphere::Sphere(Vector c_, double r_, material_t mat_, Vector blur_) : r(r_) {
  mat = mat_;
  c = c_;
  blur = blur_;
}

Vector Sphere::GetNormal(Vector v, Vector p) { return p - c; }

hit_t Sphere::GetRayIntersection(Ray ray) {
  // printf("Sphere:\n");
  // printf("Ray from %f %f %f going to %f %f %f\n", ray.p.x, ray.p.y, ray.p.z,
  // ray.v.x, ray.v.y, ray.v.z);
  hit_t hit;
  hit.r = ray;
  hit.s = this;
  hit.hit = false;
  hit.t = Ray::TMAX;

  if (!r) return hit;

  Vector moved_c = c + blur * ray.t;
  // printf("Center of sphere is now at %g %g %g for t = %g and blur = %g %g
  // %g\n",
  //        moved_c.x, moved_c.y, moved_c.z, ray.t, blur.x, blur.y, blur.z);
  Vector c_to_p = ray.p - moved_c;

  ray.Normalize();
  double disc =
      pow(Vector::Dot(ray.v, c_to_p), 2) -
      Vector::Dot(ray.v, ray.v) * (Vector::Dot(c_to_p, c_to_p) - r * r);

  if (disc < 0) {
    // printf("Ray missed\n");
    return hit;
  }

  double t_plus = Vector::Dot(ray.v * -1., c_to_p) + std::sqrt(disc);
  double t_minus = Vector::Dot(ray.v * -1., c_to_p) - std::sqrt(disc);

  if (t_plus > Ray::TMIN && t_minus > Ray::TMIN) {
    hit.t = fmin(t_plus, t_minus);
    hit.hit = true;
    // printf("Ray hit\n");
    return hit;
  } else if (t_plus > Ray::TMIN) {
    hit.t = t_plus;
    hit.hit = true;
    // printf("Ray hit\n");
    return hit;
  } else if (t_minus > Ray::TMIN) {
    hit.t = t_minus;
    hit.hit = true;
    // printf("Ray hit\n");
    return hit;
  }

  // printf("Ray missed\n");
  return hit;
}

std::vector<Vector> Sphere::GetExtents() {
  std::vector<Vector> ret;
  Vector last_c = c + blur;

  double x_min = std::min(c.x - r, last_c.x - r);
  double x_max = std::max(c.x + r, last_c.x + r);
  double y_min = std::min(c.y - r, last_c.y - r);
  double y_max = std::max(c.y + r, last_c.y + r);
  double z_min = std::min(c.z - r, last_c.z - r);
  double z_max = std::max(c.z + r, last_c.z + r);

  ret.push_back(Vector(x_min, y_min, z_min));
  ret.push_back(Vector(x_max, y_max, z_max));
  return ret;
}
