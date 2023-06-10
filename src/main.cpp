#include "utils.h"

#include <iostream>
#include <fstream>

#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "material.h"
#include "camera.h"

color ray_color(const ray &r, const hittable &world, int depth)
{
  if (depth <= 0)
    return color(0, 0, 0);

  hit_record rec;
  if (world.hit(r, 0.001, infinity, rec))
  {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return color(0, 0, 0);
  }

  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int read_int(char *var_name)
{
  // read
  int var;
  std::cout << var_name << ": ";
  std::cin >> var;
  return var;
}

int main()
{
  srand((unsigned)time(NULL));

  // IMAGE
  int image_width = read_int("IMAGE WIDTH");

  const int samples_per_pixel = 100;
  const int aspect_ratio = 16.0 / 9.0;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int max_depth = 50;

  // CAMERA
  camera view;

  // WORLD
  hittable_list world;

  auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
  auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
  auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1);

  world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
  world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
  world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
  world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

  // OUTPUT
  std::ofstream image_file;
  image_file.open("image.ppm");

  // RENDERING

  image_file << "P3\n"
             << image_width << ' ' << image_height << "\n255\n";

  // GENERATE IMAGE FROM TOP LEFT
  for (int i = image_height - 1; i >= 0; i--)
  {
    std::cout << "Currently processing file at line: " << image_height - i << "\n";
    for (int j = 0; j < image_width; j++)
    {
      color pixel_color(0, 0, 0);
      for (int k = 0; k < samples_per_pixel; k++)
      {
        auto u = (double(j) + random_double()) / (image_width - 1);
        auto v = (double(i) + random_double()) / (image_height - 1);

        auto r = view.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
      }
      write_color(image_file, pixel_color, samples_per_pixel);
    }
  }

  image_file.close();
}