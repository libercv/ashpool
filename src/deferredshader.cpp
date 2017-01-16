#include "deferredshader.hpp"
#include <GL/glew.h>    
#include <glm/glm.hpp>
#include <iostream>     
#include <string>      
#include "camera.hpp"                
#include "model.hpp"                
#include "shaderprogram.hpp"       
#include "window.hpp"             
#include "world.hpp"

DeferredShader::DeferredShader(const World *w)
    : gBufferShader{"shaders/gbuffer.vert", "shaders/gbuffer.frag"},
      lightingPassShader{"shaders/lighting.vert", "shaders/lighting.frag"},
      world {w} {
  lightingPassShader.use();
  init_pass1_gBuffer();
  init_pass2_lighting();
}

void DeferredShader::init_pass1_gBuffer() {
  glUniform1i(lightingPassShader.getUniformLocation("gPosition"), 0);
  glUniform1i(lightingPassShader.getUniformLocation("gNormal"), 1);
  glUniform1i(lightingPassShader.getUniformLocation("gAlbedoSpec"), 2);

  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

  // - Position color buffer
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::WIDTH, Window::HEIGHT, 0,
               GL_RGB, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);

  // - Normal color buffer
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Window::WIDTH, Window::HEIGHT, 0,
               GL_RGB, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);

  // - Color + Specular color buffer
  glGenTextures(1, &gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Window::WIDTH, Window::HEIGHT, 0,
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

void DeferredShader::init_pass2_lighting() {
  
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
    // const GLfloat quadratic = 1.8;
    const GLfloat linear = 0.6;
    const GLfloat quadratic = 0.2;
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
  glUniform1i(lightingPassShader.getUniformLocation("NR_LIGHTS"), world->getPointLightsNr());
}

void DeferredShader::render() {
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
  renderQuad();
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color
// targets
// and post-processing effects.
void DeferredShader::renderQuad() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  lightingPassShader.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

  if (quadVAO == 0) {
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
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
