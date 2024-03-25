/***************************************************
 * BVH
 *
 * Bounding Volume Hierarchy creation code.
 * Creates a BVH tree and if flattens it to an array
 * Based on algorith from "Physically based Rendering,
 * 2nd edition"
 *
 * TODO: Use SAH
 *
 * 2017 - Liberto Camús
 * **************************************************/

#ifndef BVH_H
#define BVH_H

#include "bbox.hpp"
#include "bvhlinearnode.hpp"
#include <memory>
#include <vector>

class Triangle;
struct BVHTreeNode;
class BVHPrimitiveInfo;

class BVH {
public:
  size_t totalNodes = 0;
  std::vector<Triangle> triangles;
  std::unique_ptr<BVHLinearNode[]> nodes_array;
  void init();

private:
  static constexpr unsigned int PRIMITIVES_PER_NODE = 4;

  // Recursive method to build the BVH tree. Uses "createLeaf"
  void recursiveBuild(BVHTreeNode *node,
                      std::vector<BVHPrimitiveInfo> &buildData, size_t start,
                      size_t end, std::vector<Triangle> &orderedPrims);

  // Creates a leaf for the tree structure
  void createLeaf(BVHTreeNode *node, std::vector<BVHPrimitiveInfo> &buildData,
                  size_t start, size_t end, std::vector<Triangle> &orderedPrims,
                  BBox &bbox, unsigned char nPrimitives);

  // Recursive method that fills "nodes_array" with the contents of the
  // BVH tree.
  size_t flattenBVHTree(const BVHTreeNode *node, size_t &offset,
                        size_t &max_depth, size_t depth);
};

#endif // BVH_H
