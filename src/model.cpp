#include "model.hpp"
#include "mesh.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

void Model::draw(const ShaderProgram &shader) const {
  for (const auto &mesh : meshes)
    mesh.draw(shader);
}

void Model::refreshUniforms(const ShaderProgram &shader) {
  for (auto &mesh : meshes)
      mesh.refreshUniforms(shader);
}

std::vector<Triangle> Model::ExportTriangles(){
  std::vector<Triangle> t;  
  for (Mesh &mesh : meshes) {
      std::vector<GLuint> ind=mesh.getIndices();        
      std::vector<Vertex> vertices = mesh.getVertices();
      for(int i=0; i<ind.size()-3; i+=3) {
        t.emplace_back(Triangle{ vertices[ind[i]].Position, 
                                 vertices[ind[i+1]].Position, 
                                 vertices[ind[i+2]].Position });   
      }      
  }
  return t;  
}


void Model::init_cl_data(){
    
    int last_vertex=0;
    int last_index=0;
    const cl_float MAX_DIST=3000.0f;
        
    cl_float3 min_pos{MAX_DIST,MAX_DIST,MAX_DIST};
    cl_float3 max_pos{-MAX_DIST,-MAX_DIST,-MAX_DIST};    
        
    int nIndMeshes=1;
        
    for (Mesh &mesh : meshes) {
        std::vector<GLuint> ind=mesh.getIndices();        
        std::copy_n(ind.data(), ind.size(), back_inserter(cl_indices));

        for(int i=3; i<ind.size()-3; i+=3) {
          if (ind[i]!=ind[i-3] && ind[i]!=ind[i-2] && ind[i]!=ind[i-1])
            if (ind[i+1]!=ind[i-3] && ind[i+1]!=ind[i-2] && ind[i+1]!=ind[i-1])
              if (ind[i+2]!=ind[i-3] && ind[i+2]!=ind[i-2] && ind[i+2]!=ind[i-1])
                nIndMeshes++;
        }
        
        auto vertices = mesh.getVertices();
        
        for (Vertex &ver : vertices) {            
            cl_vertices.emplace_back(cl_float3{ver.Position.x, ver.Position.y, ver.Position.z});
            if (ver.Position.x>max_pos.x) max_pos.x=ver.Position.x;
            if (ver.Position.x<min_pos.x) min_pos.x=ver.Position.x;
            if (ver.Position.y>max_pos.y) max_pos.y=ver.Position.y;
            if (ver.Position.y<min_pos.y) min_pos.y=ver.Position.y;
            if (ver.Position.z>max_pos.z) max_pos.z=ver.Position.z;
            if (ver.Position.z<min_pos.z) min_pos.z=ver.Position.z;
        }
        
        cl_meshes.emplace_back(cl_mesh{last_vertex, last_index, static_cast<cl_int>(vertices.size())});
        
        last_vertex +=  vertices.size();
        last_index += ind.size();    
    }
    
    std::cout << "Max positions: " << max_pos.x << ", " << max_pos.y << ", " << max_pos.z << "\n";
    std::cout << "Min positions: " << min_pos.x << ", " << min_pos.y << ", " << min_pos.z << "\n";
    std::cout << "Triangles: " << last_index/3 << "\n";
    std::cout << "Unique vertices: " << last_vertex << "\n";
    std::cout << "Total vertices: " << last_index << "\n";
    std::cout << "Vertex compression: " << (100.0f*last_vertex)/(last_index) << "\n";
    std::cout << "Independant meshes: " << nIndMeshes << "\n";
}
