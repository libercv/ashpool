#ifndef TRIANGLE_H
#define TRIANGLE_H
#include <glm/glm.hpp>

#include "bbox.hpp"
#include <CL/cl_gl.h>

class Triangle {
public:
  Triangle(glm::vec3 &vert1, glm::vec3 &vert2, glm::vec3 &vert3) : 
    v1{vert1.x, vert1.y, vert1.z}, 
    v2{vert2.x, vert2.y, vert2.z}, 
    v3{vert3.x, vert3.y, vert3.z} {} 

  cl_float3 v1, v2, v3;
  
  BBox WorldBound() const {
    return Union(BBox(glm::vec3(v1.x, v1.y, v1.z), 
                      glm::vec3(v2.x, v2.y, v2.z)), 
                 glm::vec3(v3.x, v3.y, v3.z));
  }
  
};

#endif // TRIANGLE_H
