#pragma once

#include "light.h"

class DirectionalLight : public Light {
 public:
  DirectionalLight(color_t intensity_, Vector pos_);

  Vector L(Vector point_hit);

  color_t ComputeDiffuseComponent(Vector point_hit, Vector v, Shape *s);
  color_t ComputeSpecularComponent(Vector point_hit, Vector v, Shape *s,
                                   Vector camera_p);
};
