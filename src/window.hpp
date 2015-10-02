#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <memory>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
class Window {
public:
  Window();
  ~Window();
  bool shouldClose();
  void pollEvents();
  void swapBuffers();

  static constexpr GLuint WIDTH = 800;
  static constexpr GLuint HEIGHT = 600;
  static constexpr float RATIO = (float)WIDTH / (float)HEIGHT;

private:
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mode);
  GLFWwindow *window;
};

#endif // WINDOW_H
