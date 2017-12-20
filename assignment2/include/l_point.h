#pragma once

#include "light.h"

class PointLight : public Light {
 public:
  double r;

  PointLight(color_t intensity_, Vector pos_);
  PointLight(color_t intensity_, Vector pos_, double r_);

  color_t ComputeDiffuseComponent(Vector point_hit, Vector v, Shape *s);
  color_t ComputeSpecularComponent(Vector point_hit, Vector v, Shape *s,
                                   Vector camera_p);
  bool HasArea();
  // Vector GetRandomPoint();
};
