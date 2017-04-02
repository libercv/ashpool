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
#include "hybridshader.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "model.hpp"
#include "shaderprogram.hpp"
#include "window.hpp"
#include "world.hpp"
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

HybridShader::HybridShader(World *w)
    : gBufferShader{Config::gbuffer_shader_vert.c_str(),
                    Config::gbuffer_shader_frag.c_str()},
      world{w} {

  // Initialize uniforms for the newly created Shaders
  world->initModelsUniforms(gBufferShader);

  // Initialize GBuffer and Scene textures
  init_pass1_gBuffer(); // GBuffer
  init_pass3_blit();    // Scene

  // We need GBuffer and Scene texture before initializing OpenCL
  init_pass2_lighting();
}

void HybridShader::render() {
  pass1_gBuffer();
  pass2_lighting();
  pass3_blit();
}

void HybridShader::init_pass1_gBuffer() {
  gBufferShader.use();
  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

  // - Position color buffer
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);

  // - Normal color buffer
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
               nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);

  // - Color + Specular color buffer
  glGenTextures(1, &gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
               nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoSpec, 0);

  // - Tell OpenGL which color attachments we'll use (of this framebuffer) for
  // rendering
  GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                           GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments);
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        Config::window_width, Config::window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);

  // - Finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Load OpenCL program and compile it, initialize CL memory objects,
// kernel arguments...
void HybridShader::init_pass2_lighting() {
  opencl.loadKernelFromFile(Config::lighting_kernel);
  std::cout << "Binding textures OpenGL->OpenCL\n";

  // Make accessible to OpenCL textures created with OpenGL
  cl_shared_objects[CL_SHARED_OBJECTS::GPOSITION] = opencl.createFromGLTexture(
      gPosition, CL_MEM_READ_ONLY, GBUFFER_POSITION_TEXTURE);
  cl_shared_objects[CL_SHARED_OBJECTS::GALBEDOSPEC] =
      opencl.createFromGLTexture(gAlbedoSpec, CL_MEM_READ_ONLY,
                                 GBUFFER_ALBEDO_SPEC_TEXTURE);
  cl_shared_objects[CL_SHARED_OBJECTS::GNORMAL] = opencl.createFromGLTexture(
      gNormal, CL_MEM_READ_ONLY, GBUFFER_NORMAL_TEXTURE);
  cl_shared_objects[CL_SHARED_OBJECTS::GSCENE] = opencl.createFromGLTexture(
      gSceneTexture, CL_MEM_WRITE_ONLY, GBUFFER_SCENE_TEXTURE);

  // Create point lights memory structure
  // auto lPos = world->getPointLights();
  // cl_point_lights =
  //    opencl.createBuffer(3 * sizeof(float) * lPos.size(), lPos.data());
  cl_int nr_point_lights = world->getPointLightsNr();

  // Create geometry memory structures (BVH Nodes + Triangles)
  cl_nodesbvh =
      opencl.createBuffer(world->bvh.totalNodes * sizeof(BVH::LinearBVHNode),
                          (void *)&world->bvh.nodes[0]);
  cl_primitives =
      opencl.createBuffer(world->bvh.primitives.size() * sizeof(Triangle),
                          (void *)world->bvh.primitives.data());

  // Set kernel arguments
  opencl.setKernelArg(0, sizeof(cl_mem),
                      &cl_shared_objects[CL_SHARED_OBJECTS::GALBEDOSPEC]);
  opencl.setKernelArg(1, sizeof(cl_mem),
                      &cl_shared_objects[CL_SHARED_OBJECTS::GPOSITION]);
  opencl.setKernelArg(2, sizeof(cl_mem),
                      &cl_shared_objects[CL_SHARED_OBJECTS::GNORMAL]);
  // opencl.setKernelArg(3, sizeof(cl_mem), &cl_point_lights);
  opencl.setKernelArg(4, sizeof(cl_int), &nr_point_lights);
  opencl.setKernelArg(5, sizeof(world->scene_attribs),
                      (void *)&world->scene_attribs);
  opencl.setKernelArg(6, sizeof(cl_mem), &cl_primitives);
  opencl.setKernelArg(7, sizeof(cl_mem), &cl_nodesbvh);
  // opencl.setKernelArg(8, sizeof(cl_float3), &cl_nodesbvh);
  opencl.setKernelArg(9, sizeof(cl_mem),
                      &cl_shared_objects[CL_SHARED_OBJECTS::GSCENE]);

  std::cout << "OpenCL initialized\n";
}

// Initialize the framebuffer which will show the scene
void HybridShader::init_pass3_blit() {
  glGenFramebuffers(1, &gSceneBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gSceneBuffer);

  // - Scene will be rendered here
  glGenTextures(1, &gSceneTexture);
  glBindTexture(GL_TEXTURE_2D, gSceneTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gSceneTexture, 0);

  GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, attachments);

  // - Finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Update the GBuffer
void HybridShader::pass1_gBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gBufferShader.use();

  const Camera *camera = world->getCamera();
  const std::vector<Model> *models = world->getModels();

  auto vMat = camera->getViewMatrix();
  auto loc = gBufferShader.getUniformLocation(UNIFORM_VIEW_MATRIX);
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(vMat));

  auto projMatrix = camera->getProjectionMatrix();
  auto projLoc = gBufferShader.getUniformLocation(UNIFORM_PROJECTION_MATRIX);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

  for (auto &m : *models) {
    glUniformMatrix4fv(gBufferShader.getUniformLocation(UNIFORM_MODEL_MATRIX),
                       1, GL_FALSE, glm::value_ptr(m.getModelMatrix()));
    m.draw(gBufferShader);
  }
}

// Generate final image from GBuffer + Geometry & Light information
// Update variable kernel arguments
void HybridShader::pass2_lighting() {

  // Update Point Lights (they might change)
  auto lPos = world->getPointLights();
  cl_point_lights =
      opencl.createBuffer(sizeof(PointLight) * lPos.size(), lPos.data());
  opencl.setKernelArg(3, sizeof(cl_mem), &cl_point_lights);

  // Update scene attributes
  opencl.setKernelArg(5, sizeof(world->scene_attribs),
                      (void *)&world->scene_attribs);

  // Update camera position
  glm::vec3 pos = world->getCamera()->getPosition();
  cl_float3 p = cl_float3{{pos.x, pos.y, pos.z}};
  opencl.setKernelArg(8, sizeof(cl_float3), &p);

  // Sync with OpenGL
  cl_event kernel_event;
  glFinish();
  opencl.enqueueAcquireGLObjects(CL_SHARED_OBJECTS::CL_SHARED_OBJECTS_COUNT,
                                 cl_shared_objects);

  // Execute kernel
  opencl.executeKernel(&kernel_event);

  // Sync with OpenGL
  clWaitForEvents(1, &kernel_event);
  opencl.enqueueReleaseGLObjects(CL_SHARED_OBJECTS::CL_SHARED_OBJECTS_COUNT,
                                 cl_shared_objects);
  // opencl.finish();
  clReleaseEvent(kernel_event);
  clReleaseMemObject(cl_point_lights);
}

// Copy the final image to the framebuffer
void HybridShader::pass3_blit() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gSceneBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, Config::window_width, Config::window_height, 0, 0,
                    Config::window_width, Config::window_height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

HybridShader::~HybridShader() {
  clReleaseMemObject(cl_shared_objects[CL_SHARED_OBJECTS::GALBEDOSPEC]);
  clReleaseMemObject(cl_shared_objects[CL_SHARED_OBJECTS::GPOSITION]);
  clReleaseMemObject(cl_shared_objects[CL_SHARED_OBJECTS::GNORMAL]);
  clReleaseMemObject(cl_shared_objects[CL_SHARED_OBJECTS::GSCENE]);
  clReleaseMemObject(cl_primitives);
  clReleaseMemObject(cl_nodesbvh);

  glDeleteTextures(1, &gPosition);
  glDeleteTextures(1, &gNormal);
  glDeleteTextures(1, &gAlbedoSpec);
  glDeleteTextures(1, &gSceneTexture);
  glDeleteRenderbuffers(1, &rboDepth);
  glDeleteFramebuffers(1, &gSceneBuffer);
  glDeleteFramebuffers(1, &gBuffer);
  std::cout << "Hybrid Shader destructor called\n";
}
