#include "hybridshader.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "model.hpp"
#include "shaderprogram.hpp"
#include "window.hpp"
#include "world.hpp"
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

HybridShader::HybridShader(const World *w)
    : gBufferShader{Config::gbuffer_shader_vert.c_str(),
                    Config::gbuffer_shader_frag.c_str()},
      lightingPassShader{Config::lighting_shader_vert.c_str(),
                         Config::lighting_shader_frag.c_str()},
      world{w} {

  init_pass1_gBuffer();
  init_pass2_lighting();
  init_opencl();
}

void HybridShader::init_opencl() {
  opencl.loadProgram(Config::lighting_kernel);
  std::cout << "Binding textures OpenGL->OpenCL\n";
  cl_gPosition =
      opencl.createFromGLTexture(gPosition, CL_MEM_READ_ONLY, "gPosition");
  cl_gAlbedoSpec =
      opencl.createFromGLTexture(gAlbedoSpec, CL_MEM_READ_ONLY, "gAlbedoSpec");
  cl_gNormal = opencl.createFromGLTexture(gNormal, CL_MEM_READ_ONLY, "gNormal");
  cl_gScene =
      opencl.createFromGLTexture(gSceneTexture, CL_MEM_WRITE_ONLY, "gScene");

  // Point Lights
  auto lPos = world->getPointLights();
  cl_point_lights =
      opencl.createBuffer(3 * sizeof(float) * lPos.size(), lPos.data());
  cl_int nr_point_lights = world->getPointLightsNr();

  // Geometry
  cl_mem cl_nodesbvh =
      opencl.createBuffer(world->bvh.totalNodes * sizeof(BVH::LinearBVHNode),
                          (void *)&world->bvh.nodes[0]);

  cl_mem cl_primitives =
      opencl.createBuffer(world->bvh.primitives.size() * sizeof(Triangle),
                          (void *)world->bvh.primitives.data());

  opencl.setKernelArg(0, sizeof(cl_mem), &cl_gAlbedoSpec);
  opencl.setKernelArg(1, sizeof(cl_mem), &cl_gPosition);
  opencl.setKernelArg(2, sizeof(cl_mem), &cl_gNormal);
  opencl.setKernelArg(3, sizeof(cl_mem), &cl_point_lights);
  opencl.setKernelArg(4, sizeof(cl_int), &nr_point_lights);
  opencl.setKernelArg(5, sizeof(world->scene_attribs),
                      (void *)&world->scene_attribs);
  opencl.setKernelArg(6, sizeof(cl_mem), &cl_primitives);
  opencl.setKernelArg(7, sizeof(cl_mem), &cl_nodesbvh);
  //opencl.setKernelArg(8, sizeof(cl_float3), &cl_nodesbvh);
  opencl.setKernelArg(9, sizeof(cl_mem), &cl_gScene);

  std::cout << "OpenCL initialized\n";
}

void HybridShader::init_pass1_gBuffer() {
  lightingPassShader.use();
  glUniform1i(lightingPassShader.getUniformLocation("gPosition"), 0);
  glUniform1i(lightingPassShader.getUniformLocation("gNormal"), 1);
  glUniform1i(lightingPassShader.getUniformLocation("gAlbedoSpec"), 2);

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);

  // - Color + Specular color buffer
  glGenTextures(1, &gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_FLOAT, nullptr);
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

void HybridShader::init_pass2_lighting() {

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

  GLfloat quadVertices[] = {
      // Positions        // Texture Coords
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };
  // Setup plane VAO
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (GLvoid *)nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (GLvoid *)(3 * sizeof(GLfloat)));
}

void HybridShader::updateGBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gBufferShader.use();

  const Camera *camera = world->getCamera();
  const std::vector<Model> *models = world->getModels();

  auto vMat = camera->GetViewMatrix();
  auto loc = gBufferShader.getUniformLocation("view");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(vMat));

  auto projMatrix = camera->GetProjectionMatrix();
  auto projLoc = gBufferShader.getUniformLocation("projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

  for (auto &m : *models) {
    glUniformMatrix4fv(gBufferShader.getUniformLocation("model"), 1, GL_FALSE,
                       glm::value_ptr(m.getModelMatrix()));
    m.draw(gBufferShader);
  }
}

void HybridShader::secondPass() {
  auto lPos = world->getPointLights();
  cl_point_lights =
      opencl.createBuffer(sizeof(PointLight) * lPos.size(), lPos.data());

  opencl.setKernelArg(3, sizeof(cl_mem), &cl_point_lights);
  opencl.setKernelArg(5, sizeof(world->scene_attribs),
                      (void *)&world->scene_attribs);
  
  glm::vec3 *pos=&(world->getCamera()->Position);
  cl_float3 p = cl_float3{{pos->x, pos->y, pos->z}};  
  opencl.setKernelArg(8, sizeof(cl_float3), &p);

  cl_event kernel_event;
  glFinish();
  cl_command_queue cmdQueue = opencl.get_cmd_queue();
  clEnqueueAcquireGLObjects(cmdQueue, 1, &cl_gAlbedoSpec, 0, nullptr, nullptr);
  clEnqueueAcquireGLObjects(cmdQueue, 1, &cl_gPosition, 0, nullptr, nullptr);
  clEnqueueAcquireGLObjects(cmdQueue, 1, &cl_gNormal, 0, nullptr, nullptr);
  clEnqueueAcquireGLObjects(cmdQueue, 1, &cl_gScene, 0, nullptr, nullptr);

  opencl.executeKernel(&kernel_event);
  clWaitForEvents(1, &kernel_event);

  clEnqueueReleaseGLObjects(cmdQueue, 1, &cl_gAlbedoSpec, 0, nullptr, nullptr);
  clEnqueueReleaseGLObjects(cmdQueue, 1, &cl_gPosition, 0, nullptr, nullptr);
  clEnqueueReleaseGLObjects(cmdQueue, 1, &cl_gNormal, 0, nullptr, nullptr);
  clEnqueueReleaseGLObjects(cmdQueue, 1, &cl_gScene, 0, nullptr, nullptr);
  clFinish(cmdQueue);
  clReleaseEvent(kernel_event);
}

void HybridShader::renderQuad() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gSceneBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  /* Do the copy */
  glBlitFramebuffer(0, 0, Config::window_width, Config::window_height, 0, 0,
                    Config::window_width, Config::window_height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
