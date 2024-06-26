/***************************************************
 * Material
 *
 * Stores material properties. Diffuse, ambient,
 * specular...
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Material {
public:
  glm::vec4 diffuse{0.8f, 0.8f, 0.8f, 1.0f};
  glm::vec4 ambient{0.2f, 0.2f, 0.2f, 1.0f};
  glm::vec4 specular{0.0f, 0.0f, 0.0f, 1.0f};
  glm::vec4 emissive{0.0f, 0.0f, 0.0f, 1.0f};

  GLuint shininess_uniform{0};
  GLuint diffuse_uniform{0};
  GLuint ambient_uniform{0};
  GLuint specular_uniform{0};
  GLuint tex_count_uniform{0};

  float shininess{0.0f};
  int texCount{0};
};

#endif // MATERIAL_H
