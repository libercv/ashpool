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

/**
 * @brief Camera::move Update Camera position
 * Processes input received from any keyboard-like input system. Accepts input
 * parameter in the form of camera defined ENUM (to abstract it from windowing
 * systems
 * @param direction camara defined enumeration
 * @param delta_time time in milliseconds since last movement
 */
void Camera::move(camera_movement direction, GLfloat delta_time) {
  glm::vec3 movement_vector;

  if (direction == FORWARD)
    movement_vector = this->front_vector;
  else if (direction == BACKWARD)
    movement_vector = -this->front_vector;
  else if (direction == LEFT)
    movement_vector = -this->right_vector;
  else if (direction == RIGHT)
    movement_vector = this->right_vector;

  this->position += this->mov_speed * delta_time * movement_vector;
  this->update_vectors();
}

//
/**
 * @brief Camera::rotate Rotate Camera
 * Processes input received from a mouse input system. Expects the offset value
 * in both the x and y direction.
 * @param delta_yaw
 * @param delta_pitch
 */
void Camera::rotate(GLfloat delta_yaw, GLfloat delta_pitch) {
  // Update Camera's Eurler angles
  this->yaw += delta_yaw;
  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  this->pitch = glm::clamp(this->pitch + delta_pitch, MIN_PITCH, MAX_PITCH);

  // Update Front, Right and Up Vectors using the updated Euler angles
  this->update_vectors();
}

/**
 * @brief Camera::change_fovy
 * Changes the vertical field of view angle. Equivalent to zooming
 * @param delta_fovy Signed value to shrink or stretch angle (sexagesimal)
 * Valid angle values are 1 to 45
 */
void Camera::change_fovy(GLfloat delta_fovy) {
  /// Update field of view angle. Clamp to 1-45 degrees
  this->fovy = glm::clamp(this->fovy - delta_fovy, MIN_FOVY, MAX_FOVY);

  /// Update projection matrix with new fov angle
  proj_matrix =
      glm::perspective(glm::radians(fovy), Config::window_ratio, z_near, z_far);
}

/**
 * @brief Camera::update_vectors
 * Updates viewMatrix and the front, right and up vector based on
 * the Camera's Euler angles
 */
void Camera::update_vectors() {
  constexpr glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

  /// Calculate the new Front vector
  glm::vec3 front = {
      cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
      sin(glm::radians(this->pitch)),
      sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))};
  this->front_vector = glm::normalize(front);

  /// Also re-calculate the Right and Up vector
  this->right_vector = glm::normalize(glm::cross(this->front_vector, upVector));
  this->up_vector =
      glm::normalize(glm::cross(this->right_vector, this->front_vector));

  /// Update view matrix
  this->view_matrix = glm::lookAt(
      this->position, this->position + this->front_vector, this->up_vector);
}
