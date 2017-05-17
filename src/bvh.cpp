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
#include "bvh.hpp"
#include "bvhprimitiveinfo.hpp"
#include "triangle.hpp"

#include <iostream>
#include <memory>

struct BVHTreeNode {
  BBox bounds;
  std::unique_ptr<BVHTreeNode> c0, c1;
  size_t firstPrimOffset;
  unsigned char splitAxis, nPrimitives;
  BVHTreeNode() {}

  void createChildren() {
    c0 = std::make_unique<BVHTreeNode>();
    c1 = std::make_unique<BVHTreeNode>();
  }

  BVHTreeNode *getChild0() const { return c0.get(); }
  BVHTreeNode *getChild1() const { return c1.get(); }
  void InitLeaf(size_t first, unsigned char n, const BBox &b) {
    firstPrimOffset = first;
    nPrimitives = n;
    bounds = b;
  }
  void InitInterior(unsigned char axis) {
    bounds = Union(c0->bounds, c1->bounds);
    splitAxis = axis;
    nPrimitives = 0;
  }
};

void BVH::init() {
  // Initialize buildData array for triangles (primitive number,
  // bbox and centroid)
  std::vector<BVHPrimitiveInfo> buildData;
  std::vector<Triangle> orderedPrims;
  buildData.reserve(triangles.size());
  for (size_t i = 0; i < triangles.size(); ++i) {
    BBox bbox = triangles[i].getBbox();
    buildData.push_back(BVHPrimitiveInfo(i, bbox));
  }

  // Recursively build BVH tree for triangles
  std::unique_ptr<BVHTreeNode> root = std::make_unique<BVHTreeNode>();
  recursiveBuild(root.get(), buildData, 0, triangles.size(), orderedPrims);
  std::cout << "Nodes:" << totalNodes << "\n";
  triangles.swap(orderedPrims);

  // Compute representation of depth-first traversal of BVH tree
  nodes_array = std::make_unique<BVHLinearNode[]>(totalNodes);
  size_t offset{0}, maxlevel{1};
  flattenBVHTree(root.get(), offset, maxlevel, 1);

  // Print info
  std::cout << "Size of each BVH node:" << sizeof(BVHLinearNode)
            << " bytes \n";
  std::cout << "Size of BVH structure:"
            << totalNodes * sizeof(BVHLinearNode) / 1024 << " KB\n";
  std::cout << "Size of primitives:"
            << triangles.size() * sizeof(Triangle) / 1024 << " KB\n";
  std::cout << "Max levels:" << maxlevel << "\n";
  std::cout << "Number of nodes:" << totalNodes << "\n";
  std::cout << "Number of primitives: " << triangles.size() << "\n";
}

size_t BVH::flattenBVHTree(const BVHTreeNode *node, size_t &offset,
                           size_t &max_depth, size_t depth) {
  // Update max depth if necessary
  if (max_depth < depth)
    max_depth = depth;

  // Ponter to current node in the array
  BVHLinearNode *linearNode = &nodes_array[offset];
  auto my_offset = offset;
  ++offset;

  // Update bounds
  linearNode->pMin = {
      {node->bounds.pMin.x, node->bounds.pMin.y, node->bounds.pMin.z}};
  linearNode->pMax = {
      {node->bounds.pMax.x, node->bounds.pMax.y, node->bounds.pMax.z}};

  // Fill leaf or continue recursion
  if (node->nPrimitives > 0) {
    // Leaf
    linearNode->primitivesOffset = static_cast<cl_uint>(node->firstPrimOffset);
    linearNode->nPrimitives = node->nPrimitives;
  } else {
    // Create interior flattened BVH node
    linearNode->axis = node->splitAxis;
    linearNode->nPrimitives = 0;
    flattenBVHTree(node->getChild0(), offset, max_depth, depth + 1);
    linearNode->secondChildOffset = static_cast<cl_uint>(
        flattenBVHTree(node->getChild1(), offset, max_depth, depth + 1));
  }

  return my_offset;
}

void BVH::createLeaf(BVHTreeNode *node,
                     std::vector<BVHPrimitiveInfo> &buildData, size_t start,
                     size_t end, std::vector<Triangle> &orderedPrims,
                     BBox &bbox, unsigned char nPrimitives) {
  size_t firstPrimOffset = orderedPrims.size();
  for (size_t i = start; i < end; ++i) {
    size_t primNum = buildData[i].primitiveNumber;
    orderedPrims.push_back(triangles[primNum]);
  }
  node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
}

void BVH::recursiveBuild(BVHTreeNode *node,
                         std::vector<BVHPrimitiveInfo> &buildData, size_t start,
                         size_t end, std::vector<Triangle> &orderedPrims) {
  // Update total nodes
  ++this->totalNodes;

  // Compute bounds of all primitives in BVH node
  BBox bbox;
  for (size_t i = start; i < end; ++i)
    bbox = Union(bbox, buildData[i].bounds);
  size_t nPrimitives = end - start;

  if (nPrimitives < PRIMITIVES_PER_NODE) {
    createLeaf(node, buildData, start, end, orderedPrims, bbox,
               static_cast<unsigned char>(nPrimitives));
    return;
  }

  // Compute bound of primitive centroids, choose split dimension dim
  BBox centroidBounds;
  for (size_t i = start; i < end; ++i)
    centroidBounds = Union(centroidBounds, buildData[i].centroid);
  auto dim = centroidBounds.MaximumExtent();

  // Partition primitives into two sets and build children
  size_t mid = (start + end + 1) / 2; // Truncates towards zero
  if (centroidBounds.pMax[dim] != centroidBounds.pMin[dim] && nPrimitives > 2) {
    // Partition primitives through node’s midpoint
    float pmid = .5f * (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]);
    BVHPrimitiveInfo *midPtr = std::partition(
        &buildData[start], &buildData[end - 1] + 1,
        [&pmid, &dim](const auto &a) { return a.centroid[dim] < pmid; });
    mid = midPtr - &buildData[0];
  }
  node->createChildren();
  recursiveBuild(node->getChild0(), buildData, start, mid, orderedPrims);
  recursiveBuild(node->getChild1(), buildData, mid, end, orderedPrims);
  node->InitInterior(dim);
}
