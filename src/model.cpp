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
#include "model.hpp"
#include "mesh.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

#include "config.hpp"

void Model::draw(const ShaderProgram &shader) const {
  for (const auto &mesh : meshes)
    mesh.draw(shader);
}

// Updates/sets information about its meshes into the OpenGL shader
void Model::refreshUniforms(const ShaderProgram &shader) {
  for (auto &mesh : meshes)
    mesh.refreshUniforms(shader);
}

// Exports its geometry (the vertices of all of its meshes)
// in a format to be used by the BVH acceleration structure
// and exported to OpenCL
std::vector<Triangle> Model::ExportTriangles() {
  std::vector<Triangle> t;
  for (Mesh &mesh : meshes) {
    std::vector<GLuint> ind = mesh.getIndices();
    std::vector<Vertex> vertices = mesh.getVertices();
    for (size_t i = 0; i < ind.size() - 3; i += 3) {
      t.emplace_back(Triangle{vertices[ind[i]].Position,
                              vertices[ind[i + 1]].Position,
                              vertices[ind[i + 2]].Position});
    }
  }
  return t;
}
