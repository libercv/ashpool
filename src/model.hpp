#ifndef MODEL_H
#define MODEL_H

#include <vector> // for vector
#include <glm/glm.hpp>
#include "mesh.hpp" // for Mesh
class ShaderProgram;

class Model {
public:
  Model() = delete;
  Model(std::vector<Mesh> &&m) : meshes{std::move(m)} {};
  Model &operator=(const Model &other) = delete;
  Model(const Model &other) = delete;
  Model(Model &&other)
      : meshes{std::move(other.meshes)},
        modelMatrix{std::move(other.modelMatrix)} {};

  ~Model() = default;
  void draw(const ShaderProgram &shader) const;
  void refreshUniforms(const ShaderProgram &shader);
  const glm::mat4 *getModelMatrix() { return &modelMatrix; };

private:
  std::vector<Mesh> meshes;
  glm::mat4 modelMatrix{};
};

#endif // MODEL_H
