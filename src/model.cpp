#include "model.hpp"
#include "mesh.hpp"

void Model::draw(const ShaderProgram &shader) const {
  for (const auto &mesh : meshes)
    mesh.draw(shader);
}

void Model::refreshUniforms(const ShaderProgram &shader) {
  for (auto &mesh : meshes)
    mesh.refreshUniforms(shader);
}
