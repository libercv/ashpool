/***************************************************
 * Input
 *
 * Manages input from keyboard and mouse. Calls Camera 
 * methods to update its position and orientation.
 * Uses "glfwGetWindowUserPointer" to store a pointer
 * to itself in the Window object and thus be able
 * to access its methods and properties from the callback code. 
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <memory>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "camera.hpp"

class Input {
public:
  Input(GLFWwindow *w);
  Input(GLFWwindow *w, Camera *c);
  ~Input(){};
  void setCamera(Camera *c) { camera = c; };
  void move(GLfloat deltaTime);

private:
  Camera *camera;
  bool keys[1024];
  GLfloat lastX = 400, lastY = 300;
  bool firstMouse = true;

  void init(GLFWwindow *window);
  void set_key_callback(GLFWwindow *window);
  void set_scroll_callback(GLFWwindow *window);
  void set_mouse_callback(GLFWwindow *window);
};

#endif // INPUT_H
