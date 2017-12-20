#include "l_point.h"

#include <cmath>

PointLight::PointLight(color_t intensity_, Vector pos_)
    : PointLight(intensity_, pos_, 0) {}

PointLight::PointLight(color_t intensity_, Vector pos_, double r_) : r(r_) {
  intensity = intensity_;
  pos = pos_;
}

color_t PointLight::ComputeDiffuseComponent(Vector point_hit, Vector v,
                                            Shape *s) {
  Vector n = N(point_hit, v, s);
  Vector l = L(point_hit);
  double distance_factor = std::pow((pos - point_hit).Magnitude(), 2);
  double factor = fmax(0, Vector::Dot(n, l)) / distance_factor;
  return s->mat.d * intensity * factor;
}

color_t PointLight::ComputeSpecularComponent(Vector point_hit, Vector v,
                                             Shape *s, Vector camera_p) {
  Vector r = R(point_hit, v, s);
  Vector v_ = V(point_hit, camera_p);
  double distance_factor = std::pow((pos - point_hit).Magnitude(), 2.);
  double factor =
      std::pow(fmax(0, Vector::Dot(r, v_)), s->mat.ns) / distance_factor;
  return s->mat.s * intensity * factor;
}

bool PointLight::HasArea() { return r != 0; }

// Vector PointLight::GetRandomPoint() {
//   if (!r) {
//     return pos;
//   } else {

//   }
// }
