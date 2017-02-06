#include "bvh.hpp"
#include <iostream>
#include <memory>

struct BVHBuildNode {
  BBox bounds;
  std::unique_ptr<BVHBuildNode> c0{};
  std::unique_ptr<BVHBuildNode> c1{};
  uint32_t splitAxis, firstPrimOffset, nPrimitives;

  BVHBuildNode() {}

  void InitNode() {
    auto c0_tmp = std::make_unique<BVHBuildNode>();
    c0.swap(c0_tmp);

    auto c1_tmp = std::make_unique<BVHBuildNode>();
    c1.swap(c1_tmp);
  }

  BVHBuildNode *getChild0() const { return c0.get(); }
  BVHBuildNode *getChild1() const { return c1.get(); }

  void InitLeaf(uint32_t first, uint32_t n, const BBox &b) {
    firstPrimOffset = first;
    nPrimitives = n;
    bounds = b;
  }

  void InitInterior(uint32_t axis) {
    bounds = Union(c0->bounds, c1->bounds);
    splitAxis = axis;
    nPrimitives = 0;
  }
};

struct CompareToMid {
  CompareToMid(int d, float m) {
    dim = d;
    mid = m;
  }
  int dim;
  float mid;
  bool operator()(const BVHPrimitiveInfo &a) const {
    return a.centroid[dim] < mid;
  }
};

void BVH::init() {
  // Initialize buildData array for primitives
  buildData.reserve(primitives.size());
  for (uint32_t i = 0; i < primitives.size(); ++i) {
    BBox bbox = primitives[i].WorldBound();
    buildData.push_back(BVHPrimitiveInfo(i, bbox));
  }

  // Recursively build BVH tree for primitives
  std::unique_ptr<BVHBuildNode> root = std::make_unique<BVHBuildNode>();
  recursiveBuild(root.get(), buildData, 0, primitives.size(), &totalNodes,
                 orderedPrims);
  std::cout << "Nodes:" << totalNodes << "\n";
  primitives.swap(orderedPrims);

  // Compute representation of depth-first traversal of BVH tree
  nodes = std::make_unique<LinearBVHNode[]>(totalNodes);
  uint32_t offset = 0;
  flattenBVHTree(root.get(), &offset);
}

uint32_t BVH::flattenBVHTree(const BVHBuildNode *node, uint32_t *offset) {
  LinearBVHNode *linearNode = &nodes[*offset];
  // linearNode->bounds = node->bounds;
  linearNode->pMin.x = node->bounds.pMin.x;
  linearNode->pMin.y = node->bounds.pMin.y;
  linearNode->pMin.z = node->bounds.pMin.z;
  linearNode->pMax.x = node->bounds.pMax.x;
  linearNode->pMax.y = node->bounds.pMax.y;
  linearNode->pMax.z = node->bounds.pMax.z;

  uint32_t myOffset = (*offset)++;
  if (node->nPrimitives > 0) {
    linearNode->primitivesOffset = node->firstPrimOffset;
    linearNode->nPrimitives = node->nPrimitives;
  } else {
    // Create interior flattened BVH node
    linearNode->axis = node->splitAxis;
    linearNode->nPrimitives = 0;
    flattenBVHTree(node->getChild0(), offset);
    linearNode->secondChildOffset = flattenBVHTree(node->getChild1(), offset);
  }
  return myOffset;
}

void BVH::createLeaf(BVHBuildNode *node,
                     std::vector<BVHPrimitiveInfo> &buildData, uint32_t start,
                     uint32_t end, std::vector<Triangle> &orderedPrims,
                     BBox &bbox, uint32_t nPrimitives) {
  uint32_t firstPrimOffset = orderedPrims.size();
  for (uint32_t i = start; i < end; ++i) {
    uint32_t primNum = buildData[i].primitiveNumber;
    orderedPrims.push_back(primitives[primNum]);
  }
  node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
}

void BVH::recursiveBuild(BVHBuildNode *node,
                         std::vector<BVHPrimitiveInfo> &buildData,
                         uint32_t start, uint32_t end, uint32_t *totalNodes,
                         std::vector<Triangle> &orderedPrims) {

  (*totalNodes)++;

  // Compute bounds of all primitives in BVH node
  BBox bbox;
  for (uint32_t i = start; i < end; ++i)
    bbox = Union(bbox, buildData[i].bounds);
  uint32_t nPrimitives = end - start;
  if (nPrimitives < PRIMITIVES_PER_NODE) {
    createLeaf(node, buildData, start, end, orderedPrims, bbox, nPrimitives);
  } else {
    // Compute bound of primitive centroids, choose split dimension dim
    BBox centroidBounds;
    for (uint32_t i = start; i < end; ++i)
      centroidBounds = Union(centroidBounds, buildData[i].centroid);
    int dim = centroidBounds.MaximumExtent();

    // Partition primitives into two sets and build children
    uint32_t mid = (start + end) / 2;
    if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim]) {
      createLeaf(node, buildData, start, end, orderedPrims, bbox, nPrimitives);
      return;
    }
    // Partition primitives through node’s midpoint
    if (nPrimitives == 2) {
      mid = start + 1;
    } else {
      float pmid = .5f * (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]);
      BVHPrimitiveInfo *midPtr = std::partition(
          &buildData[start], &buildData[end - 1] + 1, CompareToMid(dim, pmid));
      mid = midPtr - &buildData[0];
    }
    node->InitNode();
    recursiveBuild(node->getChild0(), buildData, start, mid, totalNodes,
                   orderedPrims);
    recursiveBuild(node->getChild1(), buildData, mid, end, totalNodes,
                   orderedPrims);
    node->InitInterior(dim);
  }
}
