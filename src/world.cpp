#include "world.hpp"
#include <memory>           // for make_unique, allocator
#include "camera.hpp"       // for Camera
#include "model.hpp"        // for Model
#include "modelloader.hpp"  // for ModelLoader

World::World() : camera{std::make_unique<Camera>()} {
  ModelLoader mLoader;
  models.emplace_back(mLoader.loadModel("models/sponza3/sponza.obj"));
};

void World::initModelsUniforms(const ShaderProgram &shader) {
  for (auto &m : models) {
    m.refreshUniforms(shader);
  };
};
