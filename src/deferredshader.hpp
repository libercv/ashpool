#ifndef DEFERREDSHADER_H
#define DEFERREDSHADER_H

#include "shaderprogram.hpp" // for ShaderProgram
#include <GL/glew.h>         // for GLuint
#include <vector>            // for vector
class Camera;
class Model;
class World;

class DeferredShader {
private:
  GLuint gBuffer, gNormal;
  GLuint gPosition, gAlbedoSpec;
  GLuint rboDepth;
  GLuint quadVAO = 0;
  GLuint quadVBO;

  ShaderProgram gBufferShader;
  ShaderProgram lightingPassShader;
  const World *world;

  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void renderQuad();

public:
  // void render(const Camera *camera, const std::vector<Model> &models);
  void render();
  const ShaderProgram &getModelShader() { return gBufferShader; };

  DeferredShader(const World *w);
  ~DeferredShader(){};
};

#endif // DEFERREDSHADER_H
