#include "camera.hpp"
#include "shaderprogram.hpp"

#include "window.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() const {
  return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
  GLfloat velocity = this->MovementSpeed * deltaTime;
  if (direction == FORWARD)
    this->Position += this->Front * velocity;
  if (direction == BACKWARD)
    this->Position -= this->Front * velocity;
  if (direction == LEFT)
    this->Position -= this->Right * velocity;
  if (direction == RIGHT)
    this->Position += this->Right * velocity;

  this->updateCameraVectors();
}

// Processes input received from a mouse input system. Expects the offset value
// in both the x and y direction.
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset,
                                  GLboolean constrainPitch) {

  xoffset *= this->MouseSensitivity;
  yoffset *= this->MouseSensitivity;

  this->Yaw += xoffset;
  this->Pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (this->Pitch > 89.0f)
      this->Pitch = 89.0f;
    if (this->Pitch < -89.0f)
      this->Pitch = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Eular angles
  this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input
// on the vertical wheel-axis
void Camera::ProcessMouseScroll(GLfloat yoffset) {
  if (this->fovy >= 1.0f && this->fovy <= 45.0f)
    this->fovy -= 3 * yoffset;
  if (this->fovy <= 1.0f)
    this->fovy = 1.0f;
  if (this->fovy >= 45.0f)
    this->fovy = 45.0f;

  projMatrix =
      glm::perspective(glm::radians(fovy), Config::window_ratio, zNear, zFar);
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors() {
  // Calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  front.y = sin(glm::radians(this->Pitch));
  front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  this->Front = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  this->Right = glm::normalize(
      glm::cross(this->Front, this->WorldUp)); // Normalize the vectors, because
                                               // their length gets closer to 0
                                               // the more you look up or down
                                               // which results in slower
                                               // movement.
  this->Up = glm::normalize(glm::cross(this->Right, this->Front));
  /*
  std::cout << "x: " << Position.x << ", y: " << Position.y
            << ", z: " << Position.z << "\n";
            */
}
