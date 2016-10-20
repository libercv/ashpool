#ifndef DEFERREDSHADER_H
#define DEFERREDSHADER_H

#include <GL/glew.h>
#include "model.hpp"
#include "shaderprogram.hpp"

class Camera;


class DeferredShader {
private:
  GLuint gBuffer, gNormal; 
  GLuint gPosition, gAlbedoSpec;
  GLuint rboDepth;
  GLuint quadVAO = 0;
  GLuint quadVBO;

  ShaderProgram gBufferShader;
  ShaderProgram lightingPassShader;

  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void renderQuad();

public:
  void render(const Camera *camera, const std::vector<Model> &models);
  const ShaderProgram &getModelShader() { return gBufferShader; };

  DeferredShader();
  ~DeferredShader(){};
};

#endif // DEFERREDSHADER_H
