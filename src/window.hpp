#ifndef WINDOW_H
#define WINDOW_H

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
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

  static constexpr GLuint WIDTH = 800;
  static constexpr GLuint HEIGHT = 600;
  static constexpr float RATIO = (float)WIDTH / (float)HEIGHT;

private:
  GLFWwindow *window;
};

#endif // WINDOW_H
