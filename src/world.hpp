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
    cl_float diffuse = 0.25f;
    cl_bool shadowsEnabled = true;
  };

  SceneAttribs scene_attribs;

  World();
  ~World() {}
  void init();

  void initModelsUniforms(const ShaderProgram &shader);

  Camera *getCamera() const { return camera.get(); }
  const std::vector<Model> *getModels() const { return &models; }
  int getPointLightsNr() const { return PointLights.size(); }
  const std::vector<PointLight> &getPointLights() const { return PointLights; }
  void update();
  BVH bvh;

private:
  // Light animation. Should be in a "trajectory" class, applied to each
  // PointLight
  /*
  float cx = -570.0f;
  float cy = 300.0f;
  float cz = -65.0f;
  float r = 150.0f;
  float delta = 0.15f;
  float luz_rad = 0.0f;
  */
  std::unique_ptr<Camera> camera;
  std::vector<Model> models;
  // lights
  std::vector<PointLight> PointLights;
};

#endif // WORLD_H
