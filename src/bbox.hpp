/***************************************************
 * BBox
 *
 * Bounding box implementation
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef BBOX_H
#define BBOX_H

#include <algorithm>
#include <glm/glm.hpp>

/**
 * @brief The BBox class
 *
 * Contains a bounding box volume and related methods
 */
class BBox {
public:
  glm::vec3 pMin, pMax; /// Further apart vertices

  BBox()
      : pMin{INFINITY, INFINITY, INFINITY}, pMax{-INFINITY, -INFINITY,
                                                 -INFINITY} {}

  BBox(const glm::vec3 &p1, const glm::vec3 &p2)
      : pMin{std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)},
        pMax{std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)} {
  }

  /**
   * @brief Union of a bounding box and a point
   * @param b Bounding box
   * @param p Point to stretch the bounding box to
   * @return New bounding box with the union
   */
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

  /**
   * @brief Union of two bounding boxes
   * @param b First bbox
   * @param b2 Second bbox
   * @return New bbox which is the union of the two
   */
  friend BBox Union(const BBox &b, const BBox &b2) {
    BBox ret = b;
    ret.pMin.x = std::min(b.pMin.x, b2.pMin.x);
    ret.pMin.y = std::min(b.pMin.y, b2.pMin.y);
    ret.pMin.z = std::min(b.pMin.z, b2.pMin.z);
    ret.pMax.x = std::max(b.pMax.x, b2.pMax.x);
    ret.pMax.y = std::max(b.pMax.y, b2.pMax.y);
    ret.pMax.z = std::max(b.pMax.z, b2.pMax.z);
    return ret;
  }

  /**
   * @brief MaximumExtent
   * Calculates the largest dimension of the cube
   * @return 0 -> x, 1 -> and 2 -> z
   */
  unsigned char MaximumExtent() const {
    glm::vec3 diag = pMax - pMin; /// Diagonal
    if (diag.x > diag.y && diag.x > diag.z)
      return 0;
    else if (diag.y > diag.z)
      return 1;

    return 2;
  }
};

#endif // BBOX_H
