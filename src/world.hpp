#ifndef WORLD_H
#define WORLD_H

#include <memory>     // for unique_ptr
#include <vector>     // for vector
#include "model.hpp"
class Camera; 
class ShaderProgram;


class World {
public:
  World();
  ~World(){};

  void initModelsUniforms(const ShaderProgram &shader);

  Camera *getCamera() const { return camera.get(); };
  const std::vector<Model> *getModels() const { return &models; };

private:
  std::unique_ptr<Camera> camera;
  std::vector<Model> models;
  // lights

};

#endif // WORLD_H
