#include "ray.h"

#include <cmath>

Ray::Ray() : Ray(Vector(), Vector(), 0) {}
Ray::Ray(Vector p_, Vector v_) : Ray(p_, v_, 0) {}
Ray::Ray(Vector p_, Vector v_, double t_) : p(p_), v(v_), t(t_) {}

Vector Ray::FindPoint(double t) { return v * t + p; }

void Ray::Normalize() { v.Normalize(); }
