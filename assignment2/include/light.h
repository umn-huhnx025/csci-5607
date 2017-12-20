#pragma once

#include "shape.h"

class Light {
 public:
  color_t intensity;
  Vector pos;

  // static constexpr color_t Black = {.r = 0, .g = 0, .b = 0};
  // static constexpr color_t White = {.r = 1, .g = 1, .b = 1};

  virtual Vector V(Vector point_hit, Vector camera_p);
  virtual Vector L(Vector point_hit);
  virtual Vector N(Vector point_hit, Vector v, Shape *s);
  virtual Vector R(Vector point_hit, Vector v, Shape *s);

  virtual color_t ComputeDiffuseComponent(Vector point_hit, Vector v,
                                          Shape *s) = 0;
  virtual color_t ComputeSpecularComponent(Vector point_hit, Vector v, Shape *s,
                                           Vector camera_p) = 0;
  virtual bool HasArea();
  // virtual color_t ComputeTransitiveComponent(material_t mat) = 0;
};
