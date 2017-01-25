#ifndef WORLD_H
#define WORLD_H

#include <memory>     // for unique_ptr
#include <vector>     // for vector
#include "model.hpp"
class Camera; 
class ShaderProgram;
#include "pointlight.hpp"
#include "triangle.hpp"
#include "bvh.hpp"


class World {
public:
  World();
  ~World(){}

  void initModelsUniforms(const ShaderProgram &shader);

  Camera *getCamera() const { return camera.get(); }
  const std::vector<Model> *getModels() const { return &models; }
  int getPointLightsNr() const { return PointLights.size(); }
  const std::vector<PointLight> &getPointLights() const { return PointLights; }
  void setPointLight(int nr, glm::vec3 pos) { PointLights.at(nr)=pos; }
  BVH bvh;

private:
  std::unique_ptr<Camera> camera;
  std::vector<Model> models;
  // lights
  std::vector<PointLight> PointLights;  
  
  
  
  
};

#endif // WORLD_H
