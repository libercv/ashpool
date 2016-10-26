#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>

class PointLight {
public:
  PointLight(glm::vec3 Position) : position{Position} {}
  PointLight(float pos_x, float pos_y, float pos_z)
      : position{glm::vec3(pos_x, pos_y, pos_z)} {}
  ~PointLight() {}

  const glm::vec3 &getPosition() const { return position; }

private:
  glm::vec3 position;
};

#endif // POINTLIGHT_H
