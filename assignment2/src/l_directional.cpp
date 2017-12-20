#include "l_directional.h"

#include <cmath>

DirectionalLight::DirectionalLight(color_t intensity_, Vector pos_) {
  intensity = intensity_;
  pos = pos_;
}

Vector DirectionalLight::L(Vector point_hit) { return pos.Normalized() * -1.; }

color_t DirectionalLight::ComputeDiffuseComponent(Vector point_hit, Vector v,
                                                  Shape *s) {
  Vector n = N(point_hit, v, s);
  Vector l = L(point_hit);
  double factor = fmax(0, Vector::Dot(n, l));
  return s->mat.d * intensity * factor;
}

color_t DirectionalLight::ComputeSpecularComponent(Vector point_hit, Vector v,
                                                   Shape *s, Vector camera_p) {
  Vector r = R(point_hit, v, s);
  Vector v1 = V(point_hit, camera_p);
  double factor = std::pow(fmax(0, Vector::Dot(r, v1)), s->mat.ns);
  return s->mat.s * intensity * factor;
}
