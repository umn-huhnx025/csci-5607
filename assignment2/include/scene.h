#pragma once

#include "bvh_node.h"
#include "image.h"
#include "light.h"
#include "ray.h"
#include "shape.h"

// #include <iostream>
#include <string>
#include <vector>

typedef struct {
  Vector p;
  Vector d;
  Vector u;
  Vector r;
  double ha;
} camera_t;

typedef struct {
  int width, height;
} resolution_t;

typedef struct {
  color_t i;
  Vector p;
} directional_light_t;

typedef struct {
  color_t color;
  Vector p, d;
  double angle1, angle2;
} spot_light_t;

typedef struct {
  double b, l, d;
} canvas_t;

class Scene {
 public:
  camera_t camera;
  double focal_length;
  double focal_plane;
  double aperture;
  double depth_of_field;
  int dof_samples;
  resolution_t resolution;
  int supersample;
  int shadow_samples;
  int blur_samples;
  std::string output_file;
  std::vector<Shape *> shapes;
  std::vector<Vector *> vertices;
  std::vector<Vector *> normals;
  color_t background;
  std::vector<Light *> lights;
  color_t ambient_light;
  int max_depth;
  canvas_t canvas;
  BVHNode *bvh_root;

  Scene(std::istream &input);

  void RayTrace();
  color_t GetColorForPixel(int i, int j);
  Ray GetRayThroughCanvas(double i, double j);
  hit_t GetRayIntersection(Ray r);
  color_t ApplyLightingModel(hit_t hit, int depth);
  color_t EvaluateRayTree(Ray r, int current_depth);
};
