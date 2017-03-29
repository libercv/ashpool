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

class Camera {
private:
  const glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
  float fovy, zNear, zFar;
  glm::vec3 Position, Front, Up, Right, WorldUp;
  GLfloat Yaw, Pitch; // Euler Angles
  glm::mat4 viewMatrix, projMatrix;
  GLfloat MovementSpeed, MouseSensitivity; // Camera options

  void updateCameraVectors();

public:
  enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

  Camera(glm::vec3 position = glm::vec3(Config::camera_position.x,
                                        Config::camera_position.y,
                                        Config::camera_position.z),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         GLfloat yaw = Config::camera_yaw, GLfloat pitch = Config::camera_pitch)
      : fovy(Config::movement_zoom), zNear(Config::camera_znear),
        zFar(Config::camera_zfar), Position(position),
        Front(glm::vec3(0.0f, 0.0f, -1.0f)), WorldUp(up), Yaw(yaw),
        Pitch(pitch),
        viewMatrix(glm::lookAt(this->Position, this->Position + this->Front,
                               this->Up)),
        projMatrix(glm::perspective(glm::radians(fovy), Config::window_ratio,
                                    zNear, zFar)),
        MovementSpeed(Config::movement_speed),
        MouseSensitivity(Config::movement_sensitivity) {
    this->updateCameraVectors();
  }

  void changeFovy(GLfloat yoffset);
  void changeOrientation(GLfloat delta_yaw, GLfloat delta_pitch,
                         GLboolean constrainPitch = true);
  void move(Camera_Movement direction, GLfloat deltaTime);

  glm::mat4 const &getViewMatrix() const { return viewMatrix; }
  glm::mat4 const &getProjectionMatrix() const { return projMatrix; }
  glm::vec3 const &getPosition() const { return Position; }
};

#endif // CAMERA_H
