/***************************************************
 * Window
 *
 * Destop window initialization, event polling and swaping
 * methods. Uses GLFW3 and GLEW
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef WINDOW_H
#define WINDOW_H

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include "config.hpp"
#include <GLFW/glfw3.h>

class Input;

class Window {
public:
  Window();
  ~Window();
  bool shouldClose();
  void pollEvents();
  void swapBuffers();
  Input createInput();

  void move();

  GLuint WIDTH = Config::window_width;
  GLuint HEIGHT = Config::window_height;
  float RATIO = (float)WIDTH / (float)HEIGHT;

private:
  GLFWwindow *window;
};

#endif // WINDOW_H
