#include "world.hpp"
#include <memory>           // for make_unique, allocator
#include "camera.hpp"       // for Camera
#include "model.hpp"        // for Model
#include "modelloader.hpp"  // for ModelLoader


World::World() : camera{std::make_unique<Camera>()} {
  ModelLoader mLoader;
  models.emplace_back(mLoader.loadModel("models/sponza3/sponza.obj"));
  //models.emplace_back(mLoader.loadModel("models/sibenik/sibenik.obj"));
  models.at(0).init_cl_data();

  PointLights.emplace_back(-4.0f, 1.5f, 2.0f);
  PointLights.emplace_back(-2.5f, 1.0f, 6.5f);    
  PointLights.emplace_back(-3.5f, 1.5f, -5.0f);
  PointLights.emplace_back(3.5f, 1.5f, 5.0f);  
  
  
  for(auto &m: models) {
    auto t=m.ExportTriangles();
    std::copy_n(t.data(), t.size(), back_inserter(bvh.primitives));
  }
  bvh.init();
  std::cout << "Triangles: " << bvh.primitives.size() << "\n";
    
}

void World::initModelsUniforms(const ShaderProgram &shader) {
  for (auto &m : models) {
    m.refreshUniforms(shader);
  };
}
