#include "world.hpp"
#include <memory>           // for make_unique, allocator
#include "camera.hpp"       // for Camera
#include "model.hpp"        // for Model
#include "modelloader.hpp"  // for ModelLoader

World::World() : camera{std::make_unique<Camera>()} {
  ModelLoader mLoader;
  models.emplace_back(mLoader.loadModel("models/sponza3/sponza.obj"));
  models.at(0).init_cl_data();
  //models.emplace_back(mLoader.loadModel("models/sibenik/sibenik.obj"));

  PointLights.emplace_back(-4.0f, 1.5f, 2.0f);  
  PointLights.emplace_back(0.0f, 2.5f, 8.0f);  
  PointLights.emplace_back(3.5f, 0.5f, 5.0f);  
  PointLights.emplace_back(3.0f, 0.5f, 6.0f);  
  PointLights.emplace_back(-2.5f, 0.5f, 3.5f);
  PointLights.emplace_back(-3.5f, 0.5f, -5.0f);
  PointLights.emplace_back(3.5f, 0.5f, -5.0f);
  

}

void World::initModelsUniforms(const ShaderProgram &shader) {
  for (auto &m : models) {
    m.refreshUniforms(shader);
  };
}
