#ifndef MODEL_H
#define MODEL_H

#include <vector> // for vector
#include <glm/glm.hpp>
#include <CL/cl_gl.h>

#include "mesh.hpp" // for Mesh
class ShaderProgram;

class Model {
public:
  typedef struct {
        cl_int vertex_offset;
        cl_int first_index;
        cl_int index_nr;
  } cl_mesh;    
    
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
  const glm::mat4 &getModelMatrix() const { return modelMatrix; };

  std::vector<cl_float3> cl_vertices;
  std::vector<cl_int> cl_indices;
  std::vector<cl_mesh> cl_meshes;
  
  void init_cl_data();
  
    
private:
  std::vector<Mesh> meshes;
  glm::mat4 modelMatrix{};
  
};

#endif // MODEL_H
