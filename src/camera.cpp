/***************************************************
 * Camera
 *
 * Controls camera position, orientation and related
 * matrices (Projection and View).
 *
 * Includes methods for moving, zooming and changing
 * orientation.
 *
 * 2017 - Liberto Camús
 * **************************************************/

#include "camera.hpp"
#include <iostream>

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::move(Camera_Movement direction, GLfloat deltaTime) {
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
  /*
  std::cout << "x: " << Position.x << ", y: " << Position.y
            << ", z: " << Position.z << "\n";
  */
}

// Processes input received from a mouse input system. Expects the offset value
// in both the x and y direction.
void Camera::changeOrientation(GLfloat delta_yaw, GLfloat delta_pitch,
                               GLboolean constrainPitch) {

  delta_yaw *= this->MouseSensitivity;
  delta_pitch *= this->MouseSensitivity;

  this->Yaw += delta_yaw;
  this->Pitch += delta_pitch;

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
void Camera::changeFovy(GLfloat yoffset) {
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
  glm::vec3 front = {
      cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch)),
      sin(glm::radians(this->Pitch)),
      sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch))};
  this->Front = glm::normalize(front);

  // Also re-calculate the Right and Up vector
  this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
  this->Up = glm::normalize(glm::cross(this->Right, this->Front));

  this->viewMatrix =
      glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}
