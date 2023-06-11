#ifndef TRIANGLE_H

#define TRIANGLE_H

#include "hittable.h"

class triangle : public hittable
{
public:
  triangle(point3 v0, point3 v1, point3 v2) : vertices{v0, v1, v2} {}

  virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;

public:
  point3 vertices[3];
};

bool triangle::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
{
  // triangle edges
  vec3 v0v1 = vertices[1] - vertices[0];
  vec3 v0v2 = vertices[2] - vertices[0];

  // triangle's plane normal,
  // plane normal == triangle normal
  vec3 N = cross(v0v1, v0v2);

  double nDotRayDirection = dot(N, r.direction());
  double kEpsilon = 0.001;
  // ray parallel to plane => ray doesn't interesct with triangle
  if (fabs(nDotRayDirection) < kEpsilon)
    return false;

  // D from the plane equation, used to later calculate t and p
  double d = -dot(N, vertices[0]);
  double t = -(dot(N, r.origin()) + d) / nDotRayDirection;

  if (t < 0)
    return false;

  if (t < t_min || t > t_max)
    return false;

  point3 P = r.at(t);

  // perform inside-outside test; as of now,
  // we know the ray intersects with the plane at p,
  // however, we need to check if p is inside the triangle
  vec3 C;

  // edge 0
  vec3 v0 = vertices[1] - vertices[0];
  vec3 v0p = P - vertices[0];
  C = cross(v0, v0p);
  if (dot(N, C) < 0)
    return false;

  // edge 1
  vec3 v1 = vertices[2] - vertices[1];
  vec3 v1p = P - vertices[1];
  C = cross(v1, v1p);
  if (dot(N, C) < 0)
    return false;

  // edge 2
  vec3 v2 = vertices[0] - vertices[2];
  vec3 v2p = P - vertices[2];
  C = cross(v2, v2p);
  if (dot(N, C) < 0)
    return false;

  // bind data to hit_record
  rec.t = t;
  rec.p = P;
  rec.normal = N;
  rec.set_face_normal(r, N);

  return true;
}

#endif