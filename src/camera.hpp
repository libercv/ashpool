#ifndef CAMERA_H
#define CAMERA_H

#include "config.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.hpp"

class ShaderProgram;

class Camera {
private:
  const glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
  float fovy;
  float zNear;
  float zFar;

  glm::vec3 position;
  glm::vec3 target;
  glm::mat4 viewMatrix;
  glm::mat4 projMatrix;

public:
  enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

  // Camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // Eular Angles
  GLfloat Yaw;
  GLfloat Pitch;
  // Camera options
  GLfloat MovementSpeed;
  GLfloat MouseSensitivity;
  // GLfloat Zoom;

  // Constructor with vectors
  Camera(glm::vec3 position = glm::vec3(Config::camera_position.x,
                                        Config::camera_position.y,
                                        Config::camera_position.z),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         GLfloat yaw = Config::camera_yaw, GLfloat pitch = Config::camera_pitch)
      : fovy(Config::movement_zoom),
        projMatrix(glm::perspective(glm::radians(fovy), Config::window_ratio,
                                    Config::camera_znear, Config::camera_zfar)),
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(Config::movement_speed),
        MouseSensitivity(Config::movement_sensitivity) {
    this->Position = position;
    this->WorldUp = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
  }
  // Constructor with scalar values
  Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY,
         GLfloat upZ, GLfloat yaw, GLfloat pitch)
      : fovy(Config::movement_zoom),
        projMatrix(glm::perspective(glm::radians(fovy), Config::window_ratio,
                                    Config::camera_znear, Config::camera_zfar)),
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(Config::movement_speed),
        MouseSensitivity(Config::movement_sensitivity) {
    this->Position = glm::vec3(posX, posY, posZ);
    this->WorldUp = glm::vec3(upX, upY, upZ);
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
  }
  void ProcessMouseScroll(GLfloat yoffset);
  void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset,
                            GLboolean constrainPitch = true);
  void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjectionMatrix() const { return projMatrix; };

private:
  void updateCameraVectors();
};

#endif // CAMERA_H
