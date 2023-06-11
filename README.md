## Ray Tracing Algorithm in C++

Implementation of the ray-tracing algorithm used in CG to render 3D scenes. The algo casts rays towards pixels, detects if a ray collides with a world object and, if so, computes the ray color which is determined by the object's material.

#### Features

- ray intersection detection for spheres and cubes
- rendition of cubes by making use of triangulation to split a cube into multiple triangles
- objects shading influenced by their associated material
- two types of materials: diffuse and metallic
- antialiasing to smooth out the rough edges around objects
