#include "scene.hpp"
#include <GL/glew.h>    // for GLfloat, glClear, etc
#include <GLFW/glfw3.h> // for glfwGetTime
#include <math.h>       // for cos, sin
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.hpp"
#include "camera.hpp"        // for Camera
#include "shaderprogram.hpp" // for Shader
#include "modelloader.hpp"
#include "window.hpp"
#include "input.hpp"

Scene::Scene(Input *i)
    : camera{std::make_unique<Camera>()}, input{i} {

  input->setCamera(camera.get());
  ModelLoader mLoader;
  models.emplace_back(mLoader.loadModel("models/sponza3/sponza.obj"));

  for (auto &m : models)
    m.refreshUniforms(shader.getModelShader());
}

void Scene::render() {
  GLfloat currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  input->move(deltaTime);

  shader.render(camera.get(), models);
}

