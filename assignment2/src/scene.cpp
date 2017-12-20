#include "scene.h"
#include "l_directional.h"
#include "l_point.h"
#include "sh_sphere.h"
#include "sh_triangle.h"
#include "sh_triangle_normal.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#define STB_IMAGE_IMPLEMENTATION  // only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION  // only place once in one .cpp files
#include "stb_image_write.h"

Scene::Scene(std::istream &input) {
  srand(444);
  camera = {
      .p = Vector(0, 0, 0),
      .d = Vector(0, 0, -1),
      .u = Vector(0, 1, 0),
      .r = Vector(1, 0, 0),
      .ha = M_PI_4,
  };
  focal_plane = 0.;
  aperture = 0.;
  depth_of_field = 0.;
  resolution = {.width = 640, .height = 480};
  focal_length = 0;
  supersample = 0;
  shadow_samples = 0;
  blur_samples = 0;
  output_file = "raytraced.bmp";

  background = {.r = 0, .g = 0, .b = 0};
  material_t current_material = {
      .a = {.r = 0, .g = 0, .b = 0},
      .d = {.r = 1, .g = 1, .b = 1},
      .s = {.r = 0, .g = 0, .b = 0},
      .ns = 5,
      .t = {.r = 0, .g = 0, .b = 0},
      .ior = 1,
      .e = {.r = 0, .g = 0, .b = 0},
  };
  // printf("Current material is %g %g %g %g %g %g %g %g %g %g %g %g %g %g\n",
  //        current_material.a.r, current_material.a.g, current_material.a.b,
  //        current_material.d.r, current_material.d.g, current_material.d.b,
  //        current_material.s.r, current_material.s.g, current_material.s.b,
  //        current_material.ns, current_material.t.r, current_material.t.g,
  //        current_material.t.b, current_material.ior);
  Vector current_blur = Vector();
  ambient_light = {.r = 0, .g = 0, .b = 0};
  max_depth = 5;

  std::string command;
  while (input >> command) {
    std::string line;
    // printf("command = %s\n", command.c_str());
    if (command[0] == '#') {
      std::getline(input, line);  // skip rest of line
      continue;
    }

    if (command == "camera") {
      double px, py, pz, dx, dy, dz, ux, uy, uz, ha;
      input >> px >> py >> pz >> dx >> dy >> dz >> ux >> uy >> uz >> ha;
      ha *= M_PI / 180.;
      Vector p = Vector(px, py, pz);
      Vector d = Vector(-dx, -dy, -dz);
      Vector u = Vector(ux, uy, uz);
      camera = {
          .p = p,
          .d = d,
          .u = u,
          .r = Vector(),
          .ha = ha,
      };
      camera.d.Normalize();
      camera.u.Normalize();
      camera.r = Vector::Cross(camera.u, camera.d).Normalized();
      camera.u = Vector::Cross(camera.d, camera.r).Normalized();

      // printf(
      //     "Camera:\nright = (%f, %f, %f)\nup    = (%f, %f, %f)\nview  = (%f,
      //     "
      //     "%f, %f)\n",
      //     camera.r.x, camera.r.y, camera.r.z, camera.u.x, camera.u.y,
      //     camera.u.z, camera.d.x, camera.d.y, camera.d.z);

      // printf(
      //     "Camera is at %g %g %g looking in direction %g %g %g with up "
      //     "vector %g %g %g and FOV %g degrees\n",
      //     px, py, pz, dx, dy, dz, ux, uy, uz, ha * 180 / M_PI * 2);

    } else if (command == "motion_blur") {
      double x, y, z;
      input >> x >> y >> z;
      current_blur = Vector(x, y, z);
    } else if (command == "blur_samples") {
      input >> blur_samples;
    } else if (command == "focal_length") {
      input >> focal_length;
      // printf("Focal length is %g\n", focal_length);
    } else if (command == "aperture") {
      input >> aperture;
      // printf("Aperture is %g\n", aperture);
    } else if (command == "depth_of_field") {
      input >> depth_of_field;
      // printf("Depth of field is %g\n", depth_of_field);
    } else if (command == "dof_samples") {
      input >> dof_samples;
      // printf("Using %d samples for depth of field\n", dof_samples);
    } else if (command == "shadow_samples") {
      input >> shadow_samples;
      // printf("Using %d samples for soft shadows\n", dof_samples);
    } else if (command == "focal_plane") {
      input >> focal_plane;
      srand(10);
      // printf("Focal plane is %g\n", focal_plane);
    } else if (command == "film_resolution") {
      input >> resolution.width >> resolution.height;

      // printf("Output resoution will be %d by %d\n", resolution.width,
      //        resolution.height);

    } else if (command == "supersample") {
      input >> supersample;
      srand(10);
      // printf("Using %d samples per pixel\n", supersample);

    } else if (command == "output_image") {
      input >> output_file;

    } else if (command == "sphere") {
      double x, y, z, r;
      input >> x >> y >> z >> r;
      Sphere *s =
          new Sphere(Vector(x, y, z), r, current_material, current_blur);
      shapes.push_back(s);

      if (current_material.e.r || current_material.e.g ||
          current_material.e.b) {
        lights.push_back(new PointLight({.r = current_material.e.r,
                                         .g = current_material.e.g,
                                         .b = current_material.e.b},
                                        Vector(x, y, z), r));
      }

      // printf("Made sphere at %g, %g, %g with radius %g\n", s->c.x, s->c.y,
      //        s->c.z, s->r);

    } else if (command == "background") {
      input >> background.r >> background.g >> background.b;

      // printf("Background is %g %g %g\n", background.r, background.g,
      //        background.b);

    } else if (command == "material") {
      double ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior, er, eg,
          eb;
      input >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb >> ns >> tr >>
          tg >> tb >> ior >> er >> eg >> eb;

      current_material = {
          .a = {.r = ar, .g = ag, .b = ab},
          .d = {.r = dr, .g = dg, .b = db},
          .s = {.r = sr, .g = sg, .b = sb},
          .ns = ns,
          .t = {.r = tr, .g = tg, .b = tb},
          .ior = ior,
          .e = {.r = er, .g = eg, .b = eb},
      };

      // printf(
      //     "Current material is %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g
      //     %g "
      //     "%g\n",
      //     current_material.a.r, current_material.a.g, current_material.a.b,
      //     current_material.d.r, current_material.d.g, current_material.d.b,
      //     current_material.s.r, current_material.s.g, current_material.s.b,
      //     current_material.ns, current_material.t.r, current_material.t.g,
      //     current_material.t.b, current_material.ior, current_material.e.r,
      //     current_material.e.g, current_material.e.b);

    } else if (command == "directional_light") {
      double r, g, b, x, y, z;
      input >> r >> g >> b >> x >> y >> z;

      // printf("Adding directional light with color %g %g %g toward %g %g
      // %g\n",
      //        r, g, b, x, y, z);

      lights.push_back(
          new DirectionalLight({.r = r, .g = g, .b = b}, Vector(x, y, z)));

    } else if (command == "point_light") {
      double r, g, b, x, y, z;
      input >> r >> g >> b >> x >> y >> z;

      // printf("Adding point light with color %g %g %g at %g %g %g\n", r, g, b,
      // x,
      //        y, z);

      lights.push_back(
          new PointLight({.r = r, .g = g, .b = b}, Vector(x, y, z)));
    } else if (command == "spot_light") {
      std::getline(input, line);  // skip rest of line

    } else if (command == "ambient_light") {
      double r, g, b;
      input >> r >> g >> b;

      // printf("Ambient light set to %g %g %g\n", r, g, b);

      color_t add = {.r = r, .g = g, .b = b};
      ambient_light = ambient_light + add;

    } else if (command == "max_depth") {
      input >> max_depth;

      // printf("Max recursion depth is %d\n", max_depth);

    } else if (command == "vertex") {
      double x, y, z;
      input >> x >> y >> z;

      // printf("Added vertex %lu at %g %g %g\n", vertices.size(), x, y, z);

      vertices.push_back(new Vector(x, y, z));

    } else if (command == "normal") {
      double x, y, z;
      input >> x >> y >> z;

      // printf("Added normal vector %d at %g %g %g\n", normals.size(), x, y,
      // z);

      normals.push_back(new Vector(x, y, z));

    } else if (command == "triangle") {
      int v1, v2, v3;
      input >> v1 >> v2 >> v3;

      // printf("Made new triangle from vertices %d %d %d\n", v1, v2, v3);

      shapes.push_back(new Triangle(*vertices.at(v1), *vertices.at(v2),
                                    *vertices.at(v3), current_material));

    } else if (command == "normal_triangle") {
      int v1, v2, v3, n1, n2, n3;
      input >> v1 >> v2 >> v3 >> n1 >> n2 >> n3;

      // printf("Made new normal triangle from vertices %d %d %d and normals %d
      // "
      //        "%d %d\n",
      //        v1, v2, v3, n1, n2, n3);

      shapes.push_back(new NormalTriangle(
          *vertices.at(v1), *vertices.at(v2), *vertices.at(v3), *normals.at(n1),
          *normals.at(n2), *normals.at(n3), current_material));
    }
  }

  double b = -resolution.height / 2.;
  double l = -resolution.width / 2.;
  double d = resolution.height / (2 * std::tan(camera.ha));
  canvas = {.b = b, .l = l, .d = d};

  // printf("Canvas: b = %g, l = %g, d = %g\n", canvas.b, canvas.l, canvas.d);

  if (focal_length) {
    focal_length = resolution.width * (focal_length / 36.);
    camera.ha = std::atan(resolution.height / 2. / focal_length);
    canvas.d = focal_length;
    // printf("Vertical half-angle is %f\n", camera.ha * 180 / M_PI);
  }

  // printf("Creating BVH...\n");
  bvh_root = new BVHNode(shapes, AXIS_X);
  // printf("Made BVH\n");
}

void Scene::RayTrace() {
  Image *image = new Image(resolution.width, resolution.height);
  int num_pixels = image->NumPixels();
  int width = image->Width();
  int height = image->Height();

  // color_t colors[num_pixels];
  int image_x, image_y;

#pragma omp parallel for
  for (int t = 0; t < num_pixels; t++) {
    int i = t % width;
    int j = t / width;

    color_t c = GetColorForPixel(i, j);

    image_x = width - 1 - i;
    image_y = height - 1 - j;

    image->GetPixel(image_x, image_y)
        .SetClamp(c.r * 255., c.g * 255., c.b * 255.);
  }

  image->Write(const_cast<char *>(output_file.c_str()));
}

color_t Scene::GetColorForPixel(int i, int j) {
  Ray r = GetRayThroughCanvas(i, j);
  color_t color{0, 0, 0};

  // printf("%d\n", dof_samples);

  if (dof_samples) {
    hit_t h = GetRayIntersection(r);
    Vector focal_point;
    if (std::abs(h.t - focal_plane) < depth_of_field / 2.) {
      return EvaluateRayTree(r, 0);
    } else if (h.t < focal_plane) {
      focal_point = r.FindPoint(focal_plane - depth_of_field);
    } else {
      focal_point = r.FindPoint(focal_plane + depth_of_field);
    }

    color_t colors[dof_samples];

#pragma omp parallel for
    for (int n = 0; n < dof_samples; n++) {
      double r_x, r_y, r_z;
      double t = n / ((double)dof_samples - 1);
      double theta = (2 * dof_samples / 8.) * M_PI * t;
      double mag = aperture * std::sin(dof_samples / 1.7 * M_PI * t);
      // printf("theta is %f, mag is %f\n", theta * 180 / M_PI, mag);

      // https://en.wikipedia.org/wiki/Rotation_matrix
      Vector u = camera.d;
      double c = std::cos(theta);
      double s = std::sin(theta);
      r_x = c + u.x * u.x * (1 - c) + u.x * u.y * (1 - c) - u.z * s +
            u.x * u.z * (1 - c) + u.y * s;
      r_y = u.y * u.x * (1 - c) + u.z * s + c + u.y * u.y * (1 - c) +
            u.y * u.z * (1 - c) - u.x * s;
      r_z = u.z * u.x * (1 - c) - u.y * s + u.z * u.y * (1 - c) + u.x * s + c +
            u.z * u.z * (1 - c);
      // printf("Rotation: %f %f %f\n", r_x, r_y, r_z);
      Vector r = (Vector(r_x, r_y, r_z) + u).Normalized() * mag;
      // printf("Random offset: %f %f %f\n", r.x, r.y, r.z);
      Vector random = camera.p - r;
      // printf("Random point:  %f %f %f\n", random.x, random.y, random.z);
      Vector vec = (focal_point - random).Normalized();
      Ray secondary = Ray(random, vec);
      colors[n] = color + EvaluateRayTree(secondary, 0);

      // Vector dir = (secondary.p - camera.p).Normalized();
      // printf("primary to secondary dir: %f %f %f, mag: %f\n", dir.x, dir.y,
      //        dir.z, (secondary.p - camera.p).Magnitude());
    }
    for (int n = 0; n < dof_samples; n++) {
      color = color + colors[n];
    }
    return color / (double)dof_samples;
  } else if (supersample) {
    for (int n = 0; n < supersample; n++) {
      double offset_x = -0.5 + rand() / (float)((RAND_MAX + 1.));
      double offset_y = -0.5 + rand() / (float)((RAND_MAX + 1.));
      // printf("offset is %f %f\n", offset_x, offset_y);
      r = GetRayThroughCanvas(i + offset_x, j + offset_y);
      color = color + EvaluateRayTree(r, 0);
    }
    return color / (double)supersample;
  } else if (blur_samples) {
    for (int n = 0; n < blur_samples; n++) {
      double t = n / (blur_samples - 1.);
      r.t = t;
      // printf("r.t = %f\n", r.t);
      color = color + EvaluateRayTree(r, 0) / (double)blur_samples;
    }
    return color;
  } else {
    color = EvaluateRayTree(r, 0);
    return color;
  }
}

Ray Scene::GetRayThroughCanvas(double i, double j) {
  double u = canvas.l + (i + 0.5);
  double v = canvas.b + (j + 0.5);

  Vector vec = camera.r * u + camera.u * v - camera.d * canvas.d;
  vec.Normalize();

  // printf("\ni, j = %g, %g\n", i, j);
  // printf("u, v = %g, %g\n", u, v);
  // printf("Ray is in direction %f %f %f from camera\n", vec.x, vec.y, vec.z);

  return Ray(camera.p, vec);
}

hit_t Scene::GetRayIntersection(Ray r) {
  return bvh_root->GetRayIntersection(r);
}

color_t Scene::ApplyLightingModel(hit_t hit, int depth) {
  Shape *s = hit.s;
  color_t c = s->mat.a * ambient_light + s->mat.e;

  Vector point_hit = hit.r.FindPoint(hit.t);
  // Vector view_direction = point_hit - hit.r.p;
  Vector v = (hit.r.p - point_hit).Normalized();
  Vector n = s->GetNormal(v, point_hit).Normalized();

  // printf("point_hit: %f %f %f\n", point_hit.x, point_hit.y, point_hit.z);
  // printf("view_direction: %f %f %f\n", view_direction.x, view_direction.y,
  //        view_direction.z);
  // printf("v: %f %f %f\n", v.x, v.y, v.z);
  // printf("n: %f %f %f\n", n.x, n.y, n.z);

  for (Light *light : lights) {
    Vector l = light->L(point_hit);
    // printf("l: %f %f %f\n", l.x, l.y, l.z);

    color_t diff = light->ComputeDiffuseComponent(point_hit, v, s);
    color_t spec = light->ComputeSpecularComponent(point_hit, v, s, hit.r.p);

    // Compute shadows
    if (light->HasArea()) {
      // printf("\n");
      int shadow_count = shadow_samples;
      for (int i = 0; i < shadow_samples; i++) {
        double r_x, r_y, r_z;
        double t = i / ((float)shadow_samples - 1);
        double theta = (2 * shadow_samples / 8.) * M_PI * t;
        double mag = static_cast<PointLight *>(light)->r *
                     std::sin(shadow_samples / 1.7 * M_PI * t);
        // printf("theta is %f, mag is %f\n", theta * 180 / M_PI, mag);

        // https://en.wikipedia.org/wiki/Rotation_matrix
        Vector u = l * -1.;
        double co = std::cos(theta);
        double si = std::sin(theta);
        r_x = co + u.x * u.x * (1 - co) + u.x * u.y * (1 - co) - u.z * si +
              u.x * u.z * (1 - co) + u.y * si;
        r_y = u.y * u.x * (1 - co) + u.z * si + co + u.y * u.y * (1 - co) +
              u.y * u.z * (1 - co) - u.x * si;
        r_z = u.z * u.x * (1 - co) - u.y * si + u.z * u.y * (1 - co) +
              u.x * si + co + u.z * u.z * (1 - co);
        Vector r = (Vector(r_x, r_y, r_z) + u).Normalized() * mag;
        Vector random = light->pos - r;
        Ray shadow(point_hit, random, hit.r.t);
        hit_t h_shadow = GetRayIntersection(shadow);
        if (h_shadow.hit) {
          Vector p = h_shadow.r.FindPoint(h_shadow.t);
          if ((p - point_hit).Magnitude() <
              (light->pos - point_hit).Magnitude()) {
            // printf("%f %f %f to %f %f %f is in shadow\n", point_hit.x,
            //        point_hit.y, point_hit.z, random.x, random.y, random.z);
            shadow_count--;
          }
        }
      }
      // printf("shadow part = %f\n", shadow_count / (double)shadow_samples);
      c = c + ((diff + spec) * (double)shadow_count / (double)shadow_samples);
    } else {
      Ray shadow(point_hit, l, hit.r.t);
      hit_t h_shadow = GetRayIntersection(shadow);
      if (h_shadow.hit) {
        // printf("Shadow ray hit something\n");
        Vector p = h_shadow.r.FindPoint(h_shadow.t);
        if ((p - point_hit).Magnitude() <
            (light->pos - point_hit).Magnitude()) {
          // printf("Shadowing\n");
          continue;
        }
      }
      c = c + diff + spec;
    }
  }

  if (s->mat.s) {
    Ray reflect = Ray(point_hit, Vector::Reflect(v, n), hit.r.t);
    // printf("Reflected ray is %f %f %f from point_hit\n", reflect.v.x,
    // reflect.v.y, reflect.v.z);
    c = c + s->mat.s * EvaluateRayTree(reflect, depth);
  }

  if (s->mat.t) {
    double ior_i, ior_r;
    Vector d = v * -1.;
    if (Vector::Dot(v, n) > 0) {
      // Going into solid
      // printf("Refraction going into sphere\n");
      ior_i = 1;
      ior_r = s->mat.ior;
    } else {
      // Going out of solid
      // printf("Refraction going out of sphere\n");
      ior_i = s->mat.ior;
      ior_r = 1;
      n = n * -1.;
    }

    Vector refract_v =
        ((d - n * Vector::Dot(d, n)) * ior_i / ior_r) -
        n * std::sqrt(1 -
                      (ior_i * ior_i * (1 - std::pow(Vector::Dot(d, n), 2))) /
                          ior_r / ior_r);
    Ray refract = Ray(point_hit, refract_v, hit.r.t);
    // printf("ior_i = %g\n", ior_i);
    // printf("ior_r = %g\n", ior_r);
    // printf("n_refrac = %f %f %f\n", n.x, n.y, n.z);
    // printf("d = %f %f %f\n", d.x, d.y, d.z);
    // printf("Refracted ray is %f %f %f from point_hit\n", refract.v.x,
    // refract.v.y,
    //        refract.v.z);
    c = c + s->mat.t * EvaluateRayTree(refract, depth);
  }

  return c;
}

color_t Scene::EvaluateRayTree(Ray r, int current_depth) {
  current_depth++;
  // printf("recursion depth is %d, max is %d\n", current_depth, max_depth);

  hit_t hit = GetRayIntersection(r);
  if (hit.hit && current_depth <= max_depth + 1) {
    // printf("Ray hit\n");
    return ApplyLightingModel(hit, current_depth);
  } else {
    // printf("Ray missed\n");
    // This is a ray from the camera that didn't hit anything
    if (current_depth == 1) return background;
    // This is a reflected/refracted ray that didn't hit anything
    else
      return color_t{0, 0, 0};
  }
}
