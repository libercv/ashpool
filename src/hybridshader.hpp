#ifndef HYBRIDSHADER_H
#define HYBRIDSHADER_H

#include "cl_init.hpp"
#include "shaderprogram.hpp" // for ShaderProgram
#include <GL/glew.h>         // for GLuint
#include <vector>            // for vector
class Camera;
class Model;
class World;

class HybridShader {
private:
  CL_Init opencl;

  // GBUFFER FRAMEBUFFER
  GLuint gBuffer;                         // Framebuffer
  GLuint gNormal, gPosition, gAlbedoSpec; // Color attachments
  GLuint rboDepth;
  GLuint quadVAO = 0;
  GLuint quadVBO;

  // OPENCL RENDER FRAMEBUFFER
  GLuint gSceneBuffer;  // Framebuffer
  GLuint gSceneTexture; // Color attachment

  // OPENGL-OPENCL Shared Textures
  cl_mem cl_gPosition, cl_gAlbedoSpec, cl_gNormal;
  cl_mem cl_gScene;
  cl_mem cl_point_lights;

  ShaderProgram gBufferShader;
  ShaderProgram lightingPassShader;
  const World *world;

  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void init_opencl();
  void renderQuad();
  void secondPass();
  void updateGBuffer();

  struct SceneAttribs {    
    cl_float diffuse;
    cl_bool shadowsEnabled;
  };

  SceneAttribs scene_attribs;

public:
  void render() {
    updateGBuffer();
    secondPass();
    renderQuad();
  }
  const ShaderProgram &getModelShader() { return gBufferShader; }

  HybridShader(const World *w);
  ~HybridShader() {}
};

#endif // HYBRIDSHADER_H
