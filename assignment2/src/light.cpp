#include "light.h"

Vector Light::V(Vector point_hit, Vector camera_p) {
  return (camera_p - point_hit).Normalized();
}

Vector Light::L(Vector point_hit) { return (pos - point_hit).Normalized(); }

Vector Light::N(Vector point_hit, Vector v, Shape *s) {
  return s->GetNormal(v, point_hit).Normalized();
}

Vector Light::R(Vector point_hit, Vector v, Shape *s) {
  return Vector::Reflect(L(point_hit), N(point_hit, v, s)).Normalized();
}

bool Light::HasArea() { return false; }
