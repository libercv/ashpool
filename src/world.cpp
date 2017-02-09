#include "world.hpp"
#include "camera.hpp"      // for Camera
#include "model.hpp"       // for Model
#include "modelloader.hpp" // for ModelLoader
#include <memory>          // for make_unique, allocator
#include <sstream>

World::World() : camera{std::make_unique<Camera>()} {
  ModelLoader mLoader;
  for (std::string model : Config::models)
    models.emplace_back(mLoader.loadModel(model));

  for (auto &m : models) {
    m.init_cl_data();
    auto t = m.ExportTriangles();
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

void World::init() {
  // PointLights
  // PointLights.emplace_back(-4.0f, 1.5f, 2.0f);

  // Point Lights
  for (std::vector<float> result : Config::point_lights) {
    /*
    std::vector<std::string> result;
    std::istringstream isss(s);
    for(std::string t; isss >> t; )
        result.push_back(t);
    */
    if (result.size() != 9)
      continue;
    PointLight p(result[0], result[1], result[2]);
    p.color = cl_float3{{result[3], result[4], result[5]}};
    p.diffuse = result[6];
    p.linear = result[7];
    p.quadratic = result[8];
    std::cout << "Quadratic:" << p.quadratic << "\n";
    PointLights.push_back(p);
  }
}

void World::update() {
  scene_attribs.shadowsEnabled = Config::option_shadows_enabled;
  /*
  luz_rad = luz_rad + 0.02f;
  if (luz_rad > 2 * 3.1415)
    luz_rad -= 2 * 3.1415;

  PointLights.at(0).position.x=cx + r * cos(luz_rad);
  PointLights.at(0).position.y=cy;
  PointLights.at(0).position.z=cz + r * sin(luz_rad);
  */

  /*
  setPointLight(
      1, glm::vec3{cx + (r + delta) * cos(luz_rad), cy, cz + r * sin(luz_rad)});
  setPointLight(2, glm::vec3{cx + (r + delta) * cos(luz_rad), cy,
                             cz + (r + delta) * sin(luz_rad)});
  setPointLight(
      3, glm::vec3{cx + r * cos(luz_rad), cy, cz + (r + delta) * sin(luz_rad)});
      */
}
