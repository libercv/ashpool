#include "hybridshader.hpp"
#include "camera.hpp"
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
    : gBufferShader{"shaders/gbuffer.vert", "shaders/gbuffer.frag"},
      lightingPassShader{"shaders/lighting.vert", "shaders/lighting.frag"},
      world{w} {

  scene_attribs.diffuse=0.1f;
  //scene_attribs.linear=0.95f;
  //scene_attribs.quadratic=1.1f;
  scene_attribs.linear=0.6f;
  scene_attribs.quadratic=0.5f;
  
  init_pass1_gBuffer();
  init_pass2_lighting();
  init_opencl();
}

void HybridShader::init_opencl() {
  opencl.loadProgram("shaders/render.cl");
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
  cl_point_lights=opencl.createBuffer(3*sizeof(float)*lPos.size(), lPos.data());
  cl_int nr_point_lights=world->getPointLightsNr();

  
  // Geometry
  const Model *m=&(world->getModels()->at(0));
  int n_meshes=m->cl_meshes.size();
  cl_mem cl_world=opencl.createBuffer(n_meshes*sizeof(Model::cl_mesh), (void *)m->cl_meshes.data());    
  
  auto cl_ind = m->cl_indices;
  cl_mem cl_indices=opencl.createBuffer(cl_ind.size()*sizeof(cl_int), (void *)cl_ind.data());    
  
  auto cl_ver = m->cl_vertices;
  cl_mem cl_vertices=opencl.createBuffer(cl_ver.size()*sizeof(cl_float3), (void *)cl_ver.data());

  cl_int cl_meshes_nr=n_meshes;
    
  opencl.setKernelArg(0, sizeof(cl_mem), &cl_gAlbedoSpec);
  opencl.setKernelArg(1, sizeof(cl_mem), &cl_gPosition);
  opencl.setKernelArg(2, sizeof(cl_mem), &cl_gNormal);
  opencl.setKernelArg(3, sizeof(cl_mem), &cl_point_lights);
  opencl.setKernelArg(4, sizeof(cl_int), &nr_point_lights);
  opencl.setKernelArg(5, sizeof(scene_attribs), (void*)&scene_attribs);
  opencl.setKernelArg(6, sizeof(cl_mem), &cl_world);
  opencl.setKernelArg(7, sizeof(cl_mem), &cl_vertices);
  opencl.setKernelArg(8, sizeof(cl_mem), &cl_indices);
  opencl.setKernelArg(9, sizeof(cl_int), &cl_meshes_nr);    
  opencl.setKernelArg(10, sizeof(cl_mem), &cl_gScene);


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
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::WIDTH, Window::HEIGHT, 0,
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Window::WIDTH, Window::HEIGHT, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);

  // - Normal color buffer
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::WIDTH, Window::HEIGHT, 0,
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Window::WIDTH, Window::HEIGHT, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);

  // - Color + Specular color buffer
  glGenTextures(1, &gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Window::WIDTH, Window::HEIGHT, 0,
               GL_RGBA, GL_FLOAT, nullptr);
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
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Window::WIDTH,
                        Window::HEIGHT);
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Window::WIDTH, Window::HEIGHT, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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

  auto lPos = world->getPointLights();
  auto lCol = glm::vec3(1.0f, 1.0f, 1.0f);
  for (unsigned int i = 0; i < lPos.size(); i++) {
    glUniform3fv(lightingPassShader.getUniformLocation(
                     "lights[" + std::to_string(i) + "].Position"),
                 1, &(lPos[i].getPosition()[0]));
    glUniform3fv(lightingPassShader.getUniformLocation(
                     "lights[" + std::to_string(i) + "].Color"),
                 1, &lCol[0]);
    // Update attenuation parameters and calculate radius
    /*
    const GLfloat constant = 1.0; // Note that we don't send this to the shader,
                                  // we assume it is always 1.0 (in our case)
    */
    // const GLfloat linear = 0.7;
    // const GLfloat quadratic = 0.2;
    const GLfloat linear = 0.6;
    const GLfloat quadratic = 1.8;
    glUniform1f(lightingPassShader.getUniformLocation(
                    "lights[" + std::to_string(i) + "].Linear"),
                linear);
    glUniform1f(lightingPassShader.getUniformLocation(
                    "lights[" + std::to_string(i) + "].Quadratic"),
                quadratic);
  }
  glUniform1f(lightingPassShader.getUniformLocation("lights[0].Linear"), 0.1f);
  glUniform1f(lightingPassShader.getUniformLocation("lights[0].Quadratic"),
              0.1f);
  auto camPos = glm::vec3(0.0f, 0.5f, 0.0f);
  glUniform3fv(lightingPassShader.getUniformLocation("viewPos"), 1, &camPos[0]);
   glUniform1i(lightingPassShader.getUniformLocation("NR_LIGHTS"),
   world->getPointLightsNr());
  //glUniform1i(lightingPassShader.getUniformLocation("NR_LIGHTS"), 0);
  // if (quadVAO == 0) {
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
  //}
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
/*
    glBindFramebuffer(GL_FRAMEBUFFER, gSceneBuffer);
    lightingPassShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  */
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
  glBlitFramebuffer(0, 0, Window::WIDTH, Window::HEIGHT, 0, 0, Window::WIDTH,
                    Window::HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
