#ifndef BVHPRIMITIVEINFO_H
#define BVHPRIMITIVEINFO_H

#include "bbox.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include <vector> // for vector

struct BVHPrimitiveInfo {
  BVHPrimitiveInfo(int pn, const BBox &b) : primitiveNumber(pn), bounds(b) {
    centroid = .5f * b.pMin + .5f * b.pMax;
  }
  int primitiveNumber;
  glm::vec3 centroid;
  BBox bounds;
};

#endif // BVHPRIMITIVEINFO_H
