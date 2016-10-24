#ifndef SCENE_H
#define SCENE_H


#include <GL/glew.h>           // for GLfloat
#include <memory>              // for unique_ptr
#include <vector>              // for vector
#include "deferredshader.hpp"  // for DeferredShader
#include "model.hpp"           // for Model
#include "shader.hpp"          // for Shader
class Camera;  // lines 10-10
class Input;  // lines 11-11

class Scene {
private:
  std::unique_ptr<Camera> camera;
  std::vector<Model> models;
  GLfloat deltaTime = 0.0f; // Time between current frame and last frame
  GLfloat lastFrame = 0.0f; // Time of last frame
  Input *input;
  Shader<DeferredShader> shader;

public:
  Scene(Input *i);
  ~Scene(){};
  void render();
  void clear(){};
};

#endif // SCENE_H
