#include <vector>

#include "triangle.h"

class polygon
{
public:
  virtual std::vector<triangle> triangulate() const = 0;
};