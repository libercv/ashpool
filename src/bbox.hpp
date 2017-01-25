#ifndef BBOX_H
#define BBOX_H

#include <GL/glew.h>      // for GLuint
#include <assimp/types.h> // for aiString
#include <algorithm>      // for move
#include <glm/glm.hpp>
#include <string> // for string
#include <vector> // for vector



class BBox {
public:
  
  glm::vec3 pMin, pMax;
  BBox() :     
    pMin{INFINITY,INFINITY,INFINITY}, 
    pMax{-INFINITY,-INFINITY,-INFINITY} {}
  BBox(const glm::vec3 &p1, const glm::vec3 &p2) : 
    pMin { std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z) } ,
    pMax { std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z) } {}
  friend BBox Union(const BBox &b, const glm::vec3 &p) {
    BBox ret = b;
    ret.pMin.x = std::min(b.pMin.x, p.x);
    ret.pMin.y = std::min(b.pMin.y, p.y);
    ret.pMin.z = std::min(b.pMin.z, p.z);
    ret.pMax.x = std::max(b.pMax.x, p.x);
    ret.pMax.y = std::max(b.pMax.y, p.y);
    ret.pMax.z = std::max(b.pMax.z, p.z);
    return ret;
  }
  friend BBox Union(const BBox &b, const BBox &b2)  {
    BBox ret = b;
    ret.pMin.x = std::min(b.pMin.x, b2.pMin.x);
    ret.pMin.y = std::min(b.pMin.y, b2.pMin.y);
    ret.pMin.z = std::min(b.pMin.z, b2.pMin.z);
    ret.pMax.x = std::max(b.pMax.x, b2.pMax.x);
    ret.pMax.y = std::max(b.pMax.y, b2.pMax.y);
    ret.pMax.z = std::max(b.pMax.z, b2.pMax.z);
    return ret;    
  }
  bool Overlaps(const BBox &b) const {
    bool x = (pMax.x >= b.pMin.x) && (pMin.x <= b.pMax.x );
    bool y = (pMax.y >= b.pMin.y) && (pMin.y <= b.pMax.y );
    bool z = (pMax.z >= b.pMin.z) && (pMin.z <= b.pMax.z );
    return (x && y && z);
  }
  bool Inside(const glm::vec3 &pt) const {
    return (pt.x >= pMin.x && pt.x <= pMax.x &&
            pt.y >= pMin.y && pt.y <= pMax.y &&
            pt.z >= pMin.z && pt.z <= pMax.z);
  }  
  int MaximumExtent() const {
    glm::vec3 diag = pMax - pMin;
    if (diag.x > diag.y && diag.x > diag.z)
      return 0;
    else if (diag.y > diag.z)
      return 1;
    
    return 2;
  }
};

#endif // BBOX_H
