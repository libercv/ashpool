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
    std::copy_n(t.data(), t.size(), back_inserter(bvh.triangles));
  }

  // Initialize BVH
  bvh.init();
}

void World::initModelsUniforms(ShaderProgram &shader) {
  for (auto &m : models) {
    m.refreshUniforms(shader);
  };
}

void World::init() {
  // Load Point Lights from "Config"
  enum pos : size_t { pos_x = 0, pos_y, pos_z, color_r, color_g, color_b,
    shininess, linear, quadratic, pos_end };

  for (std::vector<float> result : Config::point_lights) {
    PointLight p(result[pos_x], result[pos_y], result[pos_z]);
    p.color = cl_float3{{result[color_r], result[color_g], result[color_b]}};
    p.intensity = result[shininess];
    p.linear = result[linear];
    p.quadratic = result[quadratic];
    PointLights.push_back(p);
  }
}

void World::update() {
  scene_attribs.shadowsEnabled = Config::option_shadows_enabled;
}
