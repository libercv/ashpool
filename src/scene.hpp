#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>
#include "shaderprogram.hpp"

class Model;
class Camera;
class Input;

class Scene {
private:
  GLuint gBuffer, gNormal; 
  // GLuint gAlbedo, gColorSpec;
  GLuint gPosition, gAlbedoSpec;
  GLuint rboDepth;
  GLuint quadVAO = 0;
  GLuint quadVBO;
  ShaderProgram gBufferShader;
  ShaderProgram lightingPassShader;
  std::unique_ptr<Camera> camera;
  // std::unique_ptr<Model> model;
  std::vector<Model> models;
  GLfloat deltaTime = 0.0f; // Time between current frame and last frame
  GLfloat lastFrame = 0.0f;   // Time of last frame

  Input *input;


  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void renderQuad();

public:
  Scene(Input *i);
  ~Scene();
  void render();
  void clear();
};

#endif // SCENE_H
