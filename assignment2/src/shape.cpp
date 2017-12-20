#include "shape.h"

color_t::operator bool() { return r || g || b; }

color_t operator+(const color_t &a, const color_t &b) {
  return {.r = a.r + b.r, .g = a.g + b.g, .b = a.b + b.b};
}

color_t operator-(const color_t &a, const color_t &b) {
  return {.r = a.r - b.r, .g = a.g - b.g, .b = a.b - b.b};
}

color_t operator*(const color_t &a, double s) {
  return {.r = a.r * s, .g = a.g * s, .b = a.b * s};
}

color_t operator*(const color_t &a, const color_t &b) {
  return {.r = a.r * b.r, .g = a.g * b.g, .b = a.b * b.b};
}

color_t operator/(const color_t &a, double s) {
  return {.r = a.r / s, .g = a.g / s, .b = a.b / s};
}
