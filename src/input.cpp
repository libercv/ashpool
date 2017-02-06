#include "input.hpp"
#include "camera.hpp"

#include <cstring>

Input::Input(GLFWwindow *window) { init(window); }

Input::Input(GLFWwindow *window, Camera *c) : camera{c} { init(window); }

void Input::init(GLFWwindow *window) {
  glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));
  std::memset(keys, false, sizeof keys);
  set_key_callback(window);
  set_mouse_callback(window);
  set_scroll_callback(window);
}

// Moves/alters the camera->positions based on user input
void Input::move(GLfloat deltaTime) {
  // Camera controls
  if (keys[GLFW_KEY_UP])
    camera->ProcessKeyboard(Camera::FORWARD, deltaTime);
  if (keys[GLFW_KEY_DOWN])
    camera->ProcessKeyboard(Camera::BACKWARD, deltaTime);
  if (keys[GLFW_KEY_LEFT])
    camera->ProcessKeyboard(Camera::LEFT, deltaTime);
  if (keys[GLFW_KEY_RIGHT])
    camera->ProcessKeyboard(Camera::RIGHT, deltaTime);
  
  
  
}

// Is called whenever a key is pressed/released via GLFW
void Input::set_key_callback(GLFWwindow *window) {
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int /*scancode*/,
                                int action, int /*mode*/) {
    auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    if(key == GLFW_KEY_S && action == GLFW_PRESS)
      thiz->camera->shadowsEnabled=!thiz->camera->shadowsEnabled;  
    if (key >= 0 && key < 1024) {
      if (action == GLFW_PRESS)
        thiz->keys[key] = true;
      else if (action == GLFW_RELEASE)
        thiz->keys[key] = false;
    }
  });
}

void Input::set_mouse_callback(GLFWwindow *window) {
  glfwSetCursorPosCallback(
      window, [](GLFWwindow *window, double xpos, double ypos) {

        auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
        if (thiz->firstMouse) {
          thiz->lastX = xpos;
          thiz->lastY = ypos;
          thiz->firstMouse = false;
        }

        GLfloat xoffset = xpos - thiz->lastX;
        GLfloat yoffset =
            thiz->lastY -
            ypos; // Reversed since y-coordinates go from bottom to left

        thiz->lastX = xpos;
        thiz->lastY = ypos;

        thiz->camera->ProcessMouseMovement(xoffset, yoffset);
      });
}

void Input::set_scroll_callback(GLFWwindow *window) {

  glfwSetScrollCallback(
      window, [](GLFWwindow *window, double /*xoffset*/, double yoffset) {

        auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
        thiz->camera->ProcessMouseScroll(yoffset);
      });
}
