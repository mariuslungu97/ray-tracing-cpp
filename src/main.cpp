#include "utils.h"

#include <iostream>
#include <fstream>

#include "hittable_list.h"
#include "color.h"
#include "sphere.h"
#include "material.h"
#include "camera.h"
#include "cube.h"

color ray_color(const ray &r, const hittable &world, int depth)
{
  // no more light is gathered if we've outsized the recursion max depth
  if (depth <= 0)
    return color(0, 0, 0);

  hit_record rec;
  // check if ray hits any of the world objects,
  // and grab information regarding the closest object hit into rec,
  if (world.hit(r, 0.001, infinity, rec))
  {
    ray scattered;
    color attenuation;
    // check if the material of the closest hit object completely absorbs the cast ray,
    // or if it scatters a partial or the total amount of light it went in,
    // if so recursively compute the color of the scattered ray
    // and modulate it with the attenuation color of the object whose point has been hit by our casted ray
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, world, depth - 1);
    return color(0, 0, 0);
  }

  // paint the background of the scene to be a linear blend of white and blue
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0);
  return (1 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int read_int(char *var_name)
{
  // read an int from standard input and return it
  int var;
  std::cout << var_name << ": ";
  std::cin >> var;
  return var;
}

int main()
{
  // feed seed into random generator
  srand((unsigned)time(NULL));

  // IMAGE
  int image_width = read_int("IMAGE WIDTH");

  // for each pixel we are going to cast 100 nearby rays,
  // compute their ray color and add it into pixel_color
  // to then take the average, thus smoothing out the world's objects around their edges
  const int samples_per_pixel = 100;
  const int aspect_ratio = 16.0 / 9.0;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  // for each casted ray, recursively compute,
  // the ray_color of its scattered byproducts up until,
  // the scattered ray does not hit a world object or up until,
  // this recursive process > max_depth
  const int max_depth = 50;

  // CAMERA
  camera cam;

  // WORLD
  hittable_list world;

  // two diffuse materials for the ground and a central sphere,
  // and metallic materials for two spheres to the left and to the right of the central sphere
  auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
  auto material_left = make_shared<metal>(color(0.4, 0.2, 0.2), 0.3);
  auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1);

  world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
  world.add(make_shared<cube>(square(point3(-1.0, 0.5, -0.5)), square(point3(-1.0, 0.0, -0.5)), material_center));
  world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_left));
  // world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
  world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

  // OUTPUT
  std::ofstream image_file;
  image_file.open("image.ppm");

  // RENDERING
  // .ppm image header
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
        // u and v pixel sample variables used to calculate,
        // the direction of the casted ray into the scene,
        // starting with a baseline v value of 1 which influences the vertical position of the ray,
        // and a baseline u value of 0 which influences the the horizontal position of the ray,
        // thus starting from the top-left quadrant of the scene
        auto u = (double(j) + random_double()) / (image_width - 1);
        auto v = (double(i) + random_double()) / (image_height - 1);

        auto r = cam.get_ray(u, v);
        // add ray color to total pixel color to be averaged in write_color method
        pixel_color += ray_color(r, world, max_depth);
      }
      write_color(image_file, pixel_color, samples_per_pixel);
    }
  }

  image_file.close();
}