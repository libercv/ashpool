#include "camera.hpp"
#include "shaderprogram.hpp"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.hpp"

Camera::Camera()
    : projMatrix{glm::perspective(fovy, Window::RATIO, zNear, zFar)} {}

void Camera::setTarget(glm::vec3 targt) { target = targt; }

void Camera::setPosition(glm::vec3 pos) { position = pos; }

void Camera::applyProjectionMatrix(const ShaderProgram *shader) {
  auto loc = shader->getUniformLocation("projection");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}

void Camera::lookAt(glm::vec3 pos, glm::vec3 targt) {
  target = targt;
  position = pos;
  viewMatrix = glm::lookAt(position, target, upVector);
}

void Camera::applyMVP(const ShaderProgram *shader,
                      const glm::mat4 *modelMatrix) {
  auto loc = shader->getUniformLocation("mvp");
  auto mvp = projMatrix * viewMatrix * (*modelMatrix);
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));
}

void Camera::applyMV(const ShaderProgram *shader,
                     const glm::mat4 *modelMatrix) {
  auto loc = shader->getUniformLocation("mv");
  auto mv = viewMatrix * (*modelMatrix);
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mv));
}

void Camera::applyViewMatrix(const ShaderProgram *shader) {
  auto loc = shader->getUniformLocation("view");
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}
