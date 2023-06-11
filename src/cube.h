#ifndef CUBE_H
#define CUBE_H

#include "vec3.h"
#include "polygon.h"
#include "hittable.h"

class square : public hittable
{
public:
  static double edge_size;
  point3 vertices[4];

public:
  square(point3 bottom_left, bool laid_flat = true)
  {
    // laid out along the z axis
    auto top_y = laid_flat ? bottom_left.y() : bottom_left.y() + edge_size;
    auto top_z = laid_flat ? bottom_left.z() - edge_size : bottom_left.z();
    vertices[0] = bottom_left;
    vertices[1] = point3(bottom_left.x() + edge_size, bottom_left.y(), bottom_left.z());
    vertices[2] = point3(bottom_left.x() + edge_size, top_y, top_z);
    vertices[3] = point3(bottom_left.x(), top_y, top_z);
  }

  square(point3 bottom_left, point3 bottom_right, point3 top_right, point3 top_left) : vertices{bottom_left, bottom_right, top_right, top_left} {};

  point3 v0() const { return point3(vertices[0].x(), vertices[0].y(), vertices[0].z()); }
  point3 v1() const { return point3(vertices[1].x(), vertices[1].y(), vertices[1].z()); }
  point3 v2() const { return point3(vertices[2].x(), vertices[2].y(), vertices[2].z()); }
  point3 v3() const { return point3(vertices[3].x(), vertices[3].y(), vertices[3].z()); }

  std::vector<triangle> triangulate() const;
  virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
};

std::vector<triangle> square::triangulate() const
{
  std::vector<triangle> triangles;

  triangles.push_back(triangle(vertices[0], vertices[1], vertices[3]));
  triangles.push_back(triangle(vertices[2], vertices[3], vertices[1]));

  return triangles;
}

bool square::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
{
  std::vector<triangle> square_triangles = triangulate();

  for (auto square_triangle : square_triangles)
  {
    if (square_triangle.hit(r, t_min, t_max, rec))
      return true;
  }

  return false;
}

inline std::ostream &operator<<(std::ostream &out, const square &s)
{
  return out << "\nSQUARE\n"
             << "V0: " << s.v0() << "\n"
             << "V1: " << s.v1() << "\nV2: " << s.v2() << "\nV3: " << s.v3() << "\n";
}

double square::edge_size = 0.5;

class cube : public hittable
{
public:
  square top_square;
  square bottom_square;
  shared_ptr<material> mat_ptr;

public:
  cube(square t, square b, shared_ptr<material> m) : top_square(t), bottom_square(b), mat_ptr(m) {}
  std::vector<square> get_faces() const;
  virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const override;
};

std::vector<square> cube::get_faces() const
{
  square front_square(bottom_square.v0(), bottom_square.v1(), top_square.v1(), top_square.v0());
  square back_square(bottom_square.v3(), bottom_square.v2(), top_square.v2(), top_square.v3());
  square right_square(bottom_square.v1(), bottom_square.v2(), top_square.v1(), top_square.v2());
  square left_square(bottom_square.v3(), bottom_square.v0(), top_square.v0(), top_square.v3());

  std::vector<square> faces = {top_square, bottom_square, front_square, back_square, right_square, left_square};
  return faces;
}

bool cube::hit(const ray &r, double t_min, double t_max, hit_record &rec) const
{
  std::vector<square> cube_faces = get_faces();
  bool hit_anything = false;
  double t_min_temp = t_min;

  for (auto cube_face : cube_faces)
  {
    if (cube_face.hit(r, t_min_temp, t_max, rec))
    {
      t_min_temp = rec.t;
      hit_anything = true;
    }
  }

  if (hit_anything)
  {
    rec.mat_ptr = mat_ptr;
    return true;
  }

  return false;
}

#endif