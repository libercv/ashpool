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

#ifndef CAMERA_H
#define CAMERA_H

#include "config.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief The Camera class
 */
class Camera {
private:
  /// Constants
  static float constexpr MIN_PITCH{-89.0f};
  static float constexpr MAX_PITCH{89.0f};
  static float constexpr MIN_FOVY{1.0f};
  static float constexpr MAX_FOVY{45.0f};

  /// Camera state
  float fovy;          /// Vertical field of view angle
  float z_near, z_far; /// Min and Max distance to cull
  glm::vec3 position;  /// Position
  GLfloat yaw, pitch;  /// Orientation, Euler angles
  GLfloat mov_speed;   /// Camera options

  /// Calculated from state
  glm::vec3 front_vector, up_vector, right_vector; /// Unit vectors
  glm::mat4 view_matrix, proj_matrix;              /// Matrices

  /// Update calculated properties
  void update_vectors();

public:
  /// Possible movement directions
  enum camera_movement { FORWARD, BACKWARD, LEFT, RIGHT };

  /// Constructor
  Camera(glm::vec3 position = glm::vec3(Config::camera_position.x,
                                        Config::camera_position.y,
                                        Config::camera_position.z),
         GLfloat yaw = Config::camera_yaw, GLfloat pitch = Config::camera_pitch)
      : fovy(Config::movement_zoom), z_near(Config::camera_znear),
        z_far(Config::camera_zfar), position(position), yaw(yaw), pitch(pitch),
        mov_speed(Config::movement_speed / 1000.0f), /// convert to units/ms
        front_vector(glm::vec3(0.0f, 0.0f, -1.0f)),
        view_matrix(glm::lookAt(this->position,
                                this->position + this->front_vector,
                                this->up_vector)),
        proj_matrix(glm::perspective(glm::radians(fovy), Config::window_ratio,
                                     z_near, z_far)) {
    this->update_vectors();
  }

  /// Movement methods
  void change_fovy(GLfloat delta_fovy);
  void rotate(GLfloat delta_yaw, GLfloat delta_pitch);
  void move(camera_movement direction, GLfloat delta_time);

  /// Information methods
  const glm::mat4 &getViewMatrix() const { return view_matrix; }
  const glm::mat4 &getProjectionMatrix() const { return proj_matrix; }
  const glm::vec3 &getPosition() const { return position; }
};

#endif // CAMERA_H
