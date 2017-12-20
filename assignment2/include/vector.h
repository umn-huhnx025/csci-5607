#pragma once

class Vector {
 public:
  double x, y, z;

  Vector();
  Vector(double x_, double y_, double z_);

  static double Dot(Vector u, Vector v);
  static Vector Cross(Vector u, Vector v);
  static double Angle(Vector u, Vector v);
  static double Determinant(Vector v[3]);
  static Vector Reflect(Vector v, Vector normal);

  operator bool();

  double Magnitude();
  Vector Normalized();
  void Normalize();
};

Vector operator+(const Vector &u, const Vector &v);
Vector operator-(const Vector &u, const Vector &v);
Vector operator*(const Vector &u, double s);
Vector operator/(const Vector &u, double s);
