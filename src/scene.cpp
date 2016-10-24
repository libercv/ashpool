#include "scene.hpp"
#include <GL/glew.h>        // for GLfloat
#include <GLFW/glfw3.h>     // for glfwGetTime
#include <memory>           // for make_unique, unique_ptr, allocator
#include "camera.hpp"       // for Camera
#include "input.hpp"        // for Input
#include "model.hpp"        // for Model
#include "modelloader.hpp"  // for ModelLoader

Scene::Scene(Input *i) : camera{std::make_unique<Camera>()}, input{i} {
  input->setCamera(camera.get());
  ModelLoader mLoader;
  models.emplace_back(mLoader.loadModel("models/sponza3/sponza.obj"));

  for (auto &m : models) {
    m.refreshUniforms(shader.getModelShader());
  }
}

void Scene::render() {
  GLfloat currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  input->move(deltaTime);

  shader.render(camera.get(), models);
}

