/***************************************************
 * DeferredShader
 *
 * Renders the scene using OpenGL deferred shader
 * algorithm. No OpenCL involved.
 * One of the two alternative methods implemented
 * (the other one is hybridshader)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef DEFERREDSHADER_H
#define DEFERREDSHADER_H

#include "renderengine.hpp"
#include "shaderprogram.hpp" // for ShaderProgram
#include <GL/glew.h>         // for GLuint
#include <vector>            // for vector

class World;

class DeferredShader : public RenderEngine {
private:
  GLuint gBuffer;                         // Framebuffer
  GLuint gNormal, gPosition, gAlbedoSpec; // Color attachments
  GLuint rboDepth;
  GLuint quadVAO = 0;
  GLuint quadVBO;

  ShaderProgram gBufferShader;
  ShaderProgram lightingPassShader;
  World *world;

  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void pass1_gBuffer();
  void pass2_lighting();

public:
  void render();
  // const ShaderProgram &getModelShader() { return gBufferShader; }

  DeferredShader(World *w);
  ~DeferredShader();
};

#endif // DEFERREDSHADER_H
