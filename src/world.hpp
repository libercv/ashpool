/***************************************************
 * World
 *
 * Main Scene object. Contains its models, lights,
 * cameras, BVH geometry structure and options.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef WORLD_H
#define WORLD_H

#include "model.hpp"
#include <memory> // for unique_ptr
#include <vector> // for vector
class Camera;
class ShaderProgram;
#include "bvh.hpp"
#include "pointlight.hpp"
#include "triangle.hpp"

class World {
public:
  struct SceneAttribs {
    cl_float ambient;
    cl_bool shadowsEnabled;
  };
  SceneAttribs scene_attribs;

  World();
  ~World() {}
  void init();
  void initModelsUniforms(ShaderProgram &shader);

  Camera *getCamera() const { return camera.get(); }
  const std::vector<Model> *getModels() const { return &models; }
  int getPointLightsNr() const { return PointLights.size(); }
  const std::vector<PointLight> &getPointLights() const { return PointLights; }
  void update();
  BVH bvh;

private:
  std::unique_ptr<Camera> camera;
  std::vector<Model> models;
  // lights
  std::vector<PointLight> PointLights;
};

#endif // WORLD_H
