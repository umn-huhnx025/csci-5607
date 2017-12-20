#include "bvh_node.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>

#include "shape.h"

BVHNode::BVHNode(std::vector<Shape *> shapes, Axis axis) {
  int n = shapes.size();
  if (n == 1) {
    left = shapes.at(0);
    right = nullptr;
    box = new BoundingBox(left);
  } else if (n == 2) {
    left = shapes.at(0);
    right = shapes.at(1);
    box = BoundingBox::Combine(BoundingBox(left), BoundingBox(right));
  } else {
    box = new BoundingBox(INFINITY, -INFINITY, INFINITY, -INFINITY, INFINITY,
                          -INFINITY);
    double m = 0;
    for (Shape *s : shapes) {
      BoundingBox temp = BoundingBox(s);
      box = BoundingBox::Combine(box, &temp);

      // Find partition point
      switch (axis) {
        case AXIS_X:
          m += s->c.x;
          break;
        case AXIS_Y:
          m += s->c.y;
          break;
        case AXIS_Z:
          m += s->c.z;
          break;
        default: {
          fprintf(stderr, "Invalid BVH axis\n");
          exit(1);
        }
      }
    }
    m /= (double)n;

    // partition shapes
    std::vector<Shape *> left_shapes;
    std::vector<Shape *> right_shapes;
    for (Shape *s : shapes) {
      switch (axis) {
        case AXIS_X: {
          if (s->c.x < m) {
            left_shapes.push_back(s);
          } else {
            right_shapes.push_back(s);
          }
          break;
        }
        case AXIS_Y: {
          if (s->c.y < m) {
            left_shapes.push_back(s);
          } else {
            right_shapes.push_back(s);
          }
          break;
        }
        case AXIS_Z: {
          if (s->c.z < m) {
            left_shapes.push_back(s);
          } else {
            right_shapes.push_back(s);
          }
          break;
        }
        default: {
          fprintf(stderr, "Invalid BVH axis\n");
          exit(1);
        }
      }
    }

    // Create new nodes
    axis = (Axis)((axis + 1) % N_AXES);
    left = left_shapes.size() > 0 ? new BVHNode(left_shapes, axis) : nullptr;
    right = right_shapes.size() > 0 ? new BVHNode(right_shapes, axis) : nullptr;
    assert(left_shapes.size() + right_shapes.size() == shapes.size());
  }
  c = box->c;
  // printf("Made BVH Node\n");
}

Vector BVHNode::GetNormal(Vector p, Vector v) { return Vector(); }

hit_t BVHNode::GetRayIntersection(Ray r) {
  // printf("BVHNode:\n");
  // printf("Ray from %f %f %f going to %f %f %f\n", r.p.x, r.p.y, r.p.z, r.v.x,
  // r.v.y, r.v.z);
  hit_t hit;
  hit.s = this;
  hit.r = r;
  hit.t = Ray::TMAX;
  hit.hit = false;

  if (box->GetRayIntersection(r).hit) {
    // printf("Bounding box hit\n");
    hit_t hit_left, hit_right;
    hit_left.hit = false;
    hit_right.hit = false;
    if (left) {
      // printf("left node exists\n");
      hit_left = left->GetRayIntersection(r);
    }
    if (right) {
      // printf("right node exists\n");
      hit_right = right->GetRayIntersection(r);
    }
    if (hit_left.hit && hit_right.hit) {
      // hit.hit = true;
      // hit.t = fmin(hit_left.t, hit_right.t);
      // return hit;
      if (hit_left.t < hit_right.t) {
        return hit_left;
      } else {
        return hit_right;
      }
    }
    if (hit_left.hit) {
      // hit.hit = true;
      // hit.t = hit_left.t;
      return hit_left;
    }
    if (hit_right.hit) {
      // hit.hit = true;
      // hit.t = hit_right.t;
      return hit_right;
    }
    return hit;
  }
  // printf("Bounding box missed\n");
  return hit;
}

std::vector<Vector> BVHNode::GetExtents() { return box->GetExtents(); }
