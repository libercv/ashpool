#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <vector>
#include "shaderprogram.hpp"

class Model;
class Camera;

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

  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void renderQuad();

public:
  Scene();
  ~Scene();
  void render();
  void clear();
};

#endif // SCENE_H
