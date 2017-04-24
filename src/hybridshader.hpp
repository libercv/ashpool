/***************************************************
 * HybridShader
 *
 * Renders the scene in 3 passes:
 * - Generate GBuffer with OpenGL
 * - Generate final Scene texture with OpenCL using
 *    GBuffer from previous pass and geometry (BVH) and lighting information
 * - Blit the result of the second pass to the render buffer (openGL)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef HYBRIDSHADER_H
#define HYBRIDSHADER_H

#include "clkernelmanager.hpp"
#include "renderengine.hpp"
#include "shaderprogram.hpp" // for ShaderProgram
#include <GL/glew.h>         // for GLuint
#include <glm/glm.hpp>
#include <vector> // for vector
class Camera;
class Model;
class World;

class HybridShader : public RenderEngine {
private:
  CLKernelManager *opencl;

  // GBUFFER FRAMEBUFFER
  GLuint gBuffer;                         // Framebuffer
  GLuint gNormal, gPosition, gAlbedoSpec; // Color attachments
  GLuint rboDepth;

  // OPENCL RENDER FRAMEBUFFER
  GLuint gSceneBuffer;  // Framebuffer
  GLuint gSceneTexture; // Color attachment

  // OPENGL-OPENCL Shared Textures
  enum CL_SHARED_OBJECTS : uint {
    GPOSITION = 0,
    GALBEDOSPEC,
    GNORMAL,
    GSCENE,
    CL_SHARED_OBJECTS_COUNT
  };
  cl_mem cl_shared_objects[CL_SHARED_OBJECTS::CL_SHARED_OBJECTS_COUNT];

  cl_mem cl_nodesbvh, cl_primitives; //, cl_output2;
  // OPENCL kernel argument: Point Lights. The might change
  // from frame to frame, so we update it.
  cl_mem cl_point_lights;

  ShaderProgram gBufferShader;
  World *world;

  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void init_pass3_blit();
  void update_kernel_args();
  void pass1_gBuffer();
  void pass2_lighting();
  void pass3_blit();

  glm::vec3 lastCameraPosition{0, 0, 0};

  const std::string GBUFFER_POSITION_TEXTURE = "gPosition";
  const std::string GBUFFER_ALBEDO_SPEC_TEXTURE = "gAlbedoSpec";
  const std::string GBUFFER_NORMAL_TEXTURE = "gNormal";
  const std::string GBUFFER_SCENE_TEXTURE = "gScene";

  const std::string UNIFORM_MODEL_MATRIX = "model";
  const std::string UNIFORM_VIEW_MATRIX = "view";
  const std::string UNIFORM_PROJECTION_MATRIX = "projection";

public:
  void render();
  // const ShaderProgram &getModelShader() { return gBufferShader; }

  HybridShader(World *w, CLKernelManager *cl);
  ~HybridShader();
};

#endif // HYBRIDSHADER_H
