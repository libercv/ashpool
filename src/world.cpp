/***************************************************
 * World
 *
 * Main Scene object. Contains its models, lights, 
 * cameras, BVH geometry structure and options.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#include "world.hpp"
#include "camera.hpp"      // for Camera
#include "model.hpp"       // for Model
#include "modelloader.hpp" // for ModelLoader
#include <memory>          // for make_unique, allocator
#include <sstream>

World::World()
    : scene_attribs{Config::ambient, Config::option_shadows_enabled},
      camera{std::make_unique<Camera>()} {

  // Load models
  ModelLoader mLoader;
  for (std::string model : Config::models)
    models.emplace_back(mLoader.loadModel(model));

  // Export its geometry to the BVH acceleration structure
  for (auto &m : models) {
    auto t = m.ExportTriangles();
    std::copy_n(t.data(), t.size(), back_inserter(bvh.primitives));
  }

  // Initialize BVH
  bvh.init();
  std::cout << "Triangles: " << bvh.primitives.size() << "\n";
}

void World::initModelsUniforms(ShaderProgram &shader) {
  for (auto &m : models) {
    m.refreshUniforms(shader);
  };
}

void World::init() {
  // Load Point Lights from "Config"
  for (std::vector<float> result : Config::point_lights) {
    PointLight p(result[0], result[1], result[2]);
    p.color = cl_float3{{result[3], result[4], result[5]}};
    p.diffuse = result[6];
    p.linear = result[7];
    p.quadratic = result[8];
    PointLights.push_back(p);
  }
}

void World::update() {
  scene_attribs.shadowsEnabled = Config::option_shadows_enabled;
}
