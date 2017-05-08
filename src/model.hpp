/***************************************************
 * Model
 *
 * Set of meshes which constitutes an entity.
 * The model contains the "ModelMatrix", to translate,
 * spin and scale it.
 * Has a method for drawing itself (calling each
 * mesh draw methods)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef MODEL_H
#define MODEL_H

#include <CL/cl_gl.h>
#include <glm/glm.hpp>
#include <vector> // for vector

#include "mesh.hpp" // for Mesh
#include "triangle.hpp"
class ShaderProgram;

class Model {
public:
  Model() = delete;
  Model(std::vector<Mesh> &&m) : meshes{std::move(m)} {}
  Model &operator=(const Model &other) = delete;
  Model(const Model &other) = delete;
  Model(Model &&other)
      : meshes{std::move(other.meshes)}, modelMatrix{
                                             std::move(other.modelMatrix)} {}

  ~Model() = default;

  void draw(const ShaderProgram &shader) const;
  const glm::mat4 &getModelMatrix() const { return modelMatrix; }

  // Updates/sets information about its meshes into the OpenGL shader
  void refreshUniforms(const ShaderProgram &shader);

  // Exports its geometry (the vertices of all of its meshes)
  // in a format to be used by the BVH acceleration structure
  // and exported to OpenCL
  std::vector<Triangle> ExportTriangles();

private:
  std::vector<Mesh> meshes;
  glm::mat4 modelMatrix{};
};

#endif // MODEL_H
