/***************************************************
 * BVHPrimitiveInfo
 *
 * Cointains info about a primitive (triangle):
 * Its number in a "primitives" array, its centroid and
 * bounding box.
 * Used to build the BVH tree
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef BVHPRIMITIVEINFO_H
#define BVHPRIMITIVEINFO_H

#include "bbox.hpp"
#include <glm/glm.hpp>

class BVHPrimitiveInfo {
public:
  BVHPrimitiveInfo(size_t pn, const BBox &b) : primitiveNumber(pn), bounds(b) {
    centroid = .5f * b.pMin + .5f * b.pMax;
  }
  size_t primitiveNumber;
  glm::vec3 centroid;
  BBox bounds;
};

#endif // BVHPRIMITIVEINFO_H
