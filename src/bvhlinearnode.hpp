/***************************************************
 * BVHLinearNode
 *
 * Contains a node of the "linear" BVH structure,
 * prepared to be stored in an array and passed to
 * OpenCL
 *
 * 2017 - Liberto Camús
 * **************************************************/

#ifndef BVHLINEARNODE_H
#define BVHLINEARNODE_H

#include "CL/cl.h"

class BVHLinearNode {
public:
  // BBox bounds;
  cl_float3 pMin;              // 16 bytes
  cl_float3 pMax;              // 16 bytes
  union {                      // 4 bytes
    cl_uint primitivesOffset;  // leaf
    cl_uint secondChildOffset; // interior
  };
  cl_uchar nPrimitives; // 0 -> Interior . 1 byte
  cl_uchar axis;        // interior node: xyz. 1 byte
};                      // 38 bytes total

#endif // BVHLINEARNODE_H
