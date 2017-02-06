#include "world.hpp"
#include "camera.hpp"      // for Camera
#include "model.hpp"       // for Model
#include "modelloader.hpp" // for ModelLoader
#include <memory>          // for make_unique, allocator

World::World() : camera{std::make_unique<Camera>()} {
  ModelLoader mLoader;
  // models.emplace_back(mLoader.loadModel("models/sponza3/sponza.obj"));
  //models.emplace_back(mLoader.loadModel("models/sibenik/sibenik.obj"));
  models.emplace_back(mLoader.loadModel("models/sponzacry/sponza.obj"));
  models.at(0).init_cl_data();

  //PointLights.emplace_back(-4.0f, 1.5f, 2.0f);
  PointLight p(-680.0f, 1900.0f, 160.0f);
  p.color=cl_float3{1.0f,0.9f,0.8f};
  p.diffuse=1.0f;
  p.linear=0.0001f;
  p.quadratic=0.0f;
  
  
  
  PointLights.emplace_back(p);
  //PointLights.emplace_back(-570.0f, 710.0f, -565.0f);
  /*
  PointLights.emplace_back(-3.5f, 1.5f, -5.0f);
  PointLights.emplace_back(3.5f, 1.5f, 5.0f);
  */

  for (auto &m : models) {
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

void World::update() {  
  scene_attribs.shadowsEnabled=camera.get()->shadowsEnabled;  
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
