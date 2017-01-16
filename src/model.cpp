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

void Model::init_cl_data(){
    
    int last_vertex=0;
    int last_index=0;
        
    for (Mesh &mesh : meshes) {
        
        std::vector<GLuint> ind=mesh.getIndices();        
        std::copy_n(ind.data(), ind.size(), back_inserter(cl_indices));
        
        auto vertices = mesh.getVertices();
        for (Vertex &ver : vertices) 
            cl_vertices.emplace_back(cl_float3{ver.Position.x, ver.Position.y, ver.Position.z});
        
        cl_meshes.emplace_back(cl_mesh{last_vertex, last_index, static_cast<cl_int>(vertices.size())});
        
        last_vertex +=  vertices.size();
        last_index += ind.size();    
    }
}
