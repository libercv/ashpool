#ifndef BVH_H
#define BVH_H
#include "CL/cl.h"
#include "bvhprimitiveinfo.hpp"
#include "triangle.hpp"
#include <GL/glew.h>      // for GLuint
#include <algorithm>      // for move
#include <assimp/types.h> // for aiString
#include <glm/glm.hpp>
#include <memory>
#include <string> // for string
#include <vector> // for vector

struct BVHBuildNode;

class BVH {
public:
  struct LinearBVHNode {
    // BBox bounds;
    cl_float3 pMin;
    cl_float3 pMax;
    union {
      cl_uint primitivesOffset;  // leaf
      cl_uint secondChildOffset; // interior
    };
    cl_uchar nPrimitives; // 0 -> Interior
    cl_uchar axis;        // interior node: xyz    
  };

  uint32_t totalNodes = 0;

  std::vector<Triangle> primitives;
  std::vector<BVHPrimitiveInfo> buildData;
  std::unique_ptr<LinearBVHNode[]> nodes;

  void init();

private:
  const unsigned int PRIMITIVES_PER_NODE = 15;
  std::vector<Triangle> orderedPrims;
  void recursiveBuild(BVHBuildNode *node,
                      std::vector<BVHPrimitiveInfo> &buildData, uint32_t start,
                      uint32_t end, uint32_t *totalNodes,
                      std::vector<Triangle> &orderedPrims);

  void createLeaf(BVHBuildNode *node, std::vector<BVHPrimitiveInfo> &buildData,
                  uint32_t start, uint32_t end,
                  std::vector<Triangle> &orderedPrims, BBox &bbox,
                  uint32_t nPrimitives);
  uint32_t flattenBVHTree(const BVHBuildNode *node, uint32_t *offset);
};

#endif // BVH_H
