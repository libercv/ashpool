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
#include "deferredshader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "shaderprogram.hpp"
#include "window.hpp"
#include "world.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

DeferredShader::DeferredShader(World *w)
    : gBufferShader{"shaders/gbuffer.vert", "shaders/gbuffer.frag"},
      lightingPassShader{"shaders/lighting.vert", "shaders/lighting.frag"},
      world{w} {

  world->initModelsUniforms(gBufferShader);

  init_pass1_gBuffer();
  init_pass2_lighting();
}

DeferredShader::~DeferredShader() {
  glDeleteTextures(1, &gPosition);
  glDeleteTextures(1, &gNormal);
  glDeleteTextures(1, &gAlbedoSpec);
  glDeleteRenderbuffers(1, &rboDepth);
  glDeleteFramebuffers(1, &gBuffer);
}

// Initialize GBuffer -> textures(albedo+specular, normals, positions)
//                       and render buffer
void DeferredShader::init_pass1_gBuffer() {
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

// Initialize lighting pass -> uniforms for textures
//                             quad for rendering final scene
//                             pass point lights information to the shader
void DeferredShader::init_pass2_lighting() {
  lightingPassShader.use();
  glUniform1i(lightingPassShader.getUniformLocation("gPosition"), 0);
  glUniform1i(lightingPassShader.getUniformLocation("gNormal"), 1);
  glUniform1i(lightingPassShader.getUniformLocation("gAlbedoSpec"), 2);
  glUniform1f(lightingPassShader.getUniformLocation("ambient"),
              Config::ambient);

  constexpr GLfloat quadVertices[] = {
      // Positions         // Texture Coords
      -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
      1.0f,   1.0f, 0.0f,  1.0f, 1.0f,
      1.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
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

  // Pass point light information to the shader
  auto pLights = world->getPointLights();
  glUniform1i(lightingPassShader.getUniformLocation("NR_LIGHTS"),
              static_cast<GLint>(world->getPointLightsNr()));
  for (unsigned int i = 0; i < pLights.size(); i++) {
    PointLight p = pLights[i];
    glm::vec3 lightPos = {p.position.x, p.position.y, p.position.z};
    glm::vec3 lightCol = {p.color.x, p.color.y, p.color.z};

    glUniform3fv(lightingPassShader.getUniformLocation(
                     "lights[" + std::to_string(i) + "].Position"),
                 1, &lightPos[0]);
    glUniform3fv(lightingPassShader.getUniformLocation(
                     "lights[" + std::to_string(i) + "].Color"),
                 1, &lightCol[0]);
    glUniform1f(lightingPassShader.getUniformLocation(
                    "lights[" + std::to_string(i) + "].Linear"),
                p.linear);
    glUniform1f(lightingPassShader.getUniformLocation(
                    "lights[" + std::to_string(i) + "].Quadratic"),
                p.quadratic);
    glUniform1f(lightingPassShader.getUniformLocation(
                    "lights[" + std::to_string(i) + "].Intensity"),
                p.intensity);
  }
}

// Main method, called once every frame
void DeferredShader::render() {
  pass1_gBuffer();
  pass2_lighting();
}

// Produces GBuffer textures
void DeferredShader::pass1_gBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gBufferShader.use();

  const Camera *camera = world->getCamera();
  const std::vector<Model> *models = world->getModels();

  auto vMat = camera->getViewMatrix();
  auto loc = gBufferShader.getUniformLocation("view");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(vMat));

  auto projMatrix = camera->getProjectionMatrix();
  auto projLoc = gBufferShader.getUniformLocation("projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

  for (auto &m : *models) {
    glUniformMatrix4fv(gBufferShader.getUniformLocation("model"), 1, GL_FALSE,
                       glm::value_ptr(m.getModelMatrix()));
    m.draw(gBufferShader);
  }
}

// Renders the scene using light information and GBuffer textures
void DeferredShader::pass2_lighting() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  lightingPassShader.use();

  // Update camera position
  auto camPos = world->getCamera()->getPosition();
  glUniform3fv(lightingPassShader.getUniformLocation("viewPos"), 1, &camPos[0]);

  // Bind GBuffer textures as input
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

  // Draw the scene
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
