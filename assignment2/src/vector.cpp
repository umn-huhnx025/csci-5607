#include "vector.h"

#include <cmath>
#include <cstdio>

Vector::Vector() : x(0), y(0), z(0) {}

Vector::Vector(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

double Vector::Dot(Vector u, Vector v) {
  // printf("Dot product of <%g, %g, %g> and <%g, %g, %g> is %g\n", u.x, u.y,
  // u.z,
  //        v.x, v.y, v.z, u.x * v.x + u.y * v.y + u.z * v.z);
  return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vector Vector::Cross(Vector u, Vector v) {
  // printf("Cross product of <%g, %g, %g> and <%g, %g, %g> is <%g, %g, %g>\n",
  //        u.x, u.y, u.z, v.x, v.y, v.z, u.y * v.z - u.z * v.y, u.z * v.x - u.x
  //        * v.z, u.x * v.y - u.y * v.x);
  return Vector(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z,
                u.x * v.y - u.y * v.x);
}

double Vector::Angle(Vector u, Vector v) {
  return std::acos(Vector::Dot(u, v) / (u.Magnitude() * v.Magnitude()));
}

double Vector::Determinant(Vector v[3]) {
  // v[0].x v[1].x v[2].x
  // v[0].y v[1].y v[2].y
  // v[0].z v[1].z v[2].z
  return v[0].x * (v[1].y * v[2].z - v[2].y * v[1].z) -
         v[1].x * (v[0].y * v[2].z - v[2].y * v[0].z) +
         v[2].x * (v[0].y * v[1].z - v[1].y * v[0].z);
}

Vector Vector::Reflect(Vector v, Vector normal) {
  return normal * 2. * Vector::Dot(normal, v) - v;
}

double Vector::Magnitude() { return std::sqrt(x * x + y * y + z * z); }

Vector Vector::Normalized() { return *this / Magnitude(); }

void Vector::Normalize() { *this = this->Normalized(); }

Vector::operator bool() { return x || y || z; }

Vector operator+(const Vector &u, const Vector &v) {
  return Vector(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vector operator-(const Vector &u, const Vector &v) {
  return Vector(u.x - v.x, u.y - v.y, u.z - v.z);
}

Vector operator*(const Vector &u, double s) {
  return Vector(u.x * s, u.y * s, u.z * s);
}

Vector operator/(const Vector &u, double s) {
  return Vector(u.x / s, u.y / s, u.z / s);
}
