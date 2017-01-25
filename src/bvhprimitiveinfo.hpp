#ifndef BVHPRIMITIVEINFO_H
#define BVHPRIMITIVEINFO_H

#include <glm/glm.hpp>
#include <vector> // for vector
#include <iostream>
#include "bbox.hpp"

struct BVHPrimitiveInfo {
  BVHPrimitiveInfo(int pn, const BBox &b)
    : primitiveNumber(pn), bounds(b) {
    centroid = .5f * b.pMin + .5f * b.pMax;    
  }
  int primitiveNumber;
  glm::vec3 centroid;
  BBox bounds;
};


#endif // BVHPRIMITIVEINFO_H
