#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>
#include "deferredshader.hpp"

class Model;
class Camera;
class Input;

class Scene {
private:
  std::unique_ptr<Camera> camera;
  std::vector<Model> models;
  GLfloat deltaTime = 0.0f; // Time between current frame and last frame
  GLfloat lastFrame = 0.0f;   // Time of last frame
  Input *input;
  DeferredShader shader;

public:
  Scene(Input *i);
  ~Scene(){};
  void render();
  void clear(){};
};

#endif // SCENE_H
