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
    camera->move(Camera::FORWARD, deltaTime);
  if (keys[GLFW_KEY_DOWN])
    camera->move(Camera::BACKWARD, deltaTime);
  if (keys[GLFW_KEY_LEFT])
    camera->move(Camera::LEFT, deltaTime);
  if (keys[GLFW_KEY_RIGHT])
    camera->move(Camera::RIGHT, deltaTime);
}

// Is called whenever a key is pressed/released via GLFW
void Input::set_key_callback(GLFWwindow *window) {
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int /*scancode*/,
                                int action, int /*mode*/) {
    auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      Config::option_shadows_enabled = !Config::option_shadows_enabled;
      if (Config::option_shadows_enabled)
        std::cout << "Shadows ON \n";
      else
        std::cout << "Shadows OFF \n";
    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS &&
        Config::rendering_method != Config::HYBRID_CPU) {
      Config::option_rendering_method_change_requested = true;
      Config::rendering_method = Config::HYBRID_CPU;
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS &&
        Config::rendering_method != Config::HYBRID) {
      if(Config::option_opencl_available) {
        Config::option_rendering_method_change_requested = true;
        Config::rendering_method = Config::HYBRID;        
      } else {
        std::cout << "OpenCL Shader not available\n";        
      }
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS &&
        Config::rendering_method != Config::DEFERRED) {
      Config::option_rendering_method_change_requested = true;
      Config::rendering_method = Config::DEFERRED;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
      Config::option_statistics_requested=true;      
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
      Config::option_reset_statistics_requested=true;      
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
      glm::vec3 pos = thiz->camera->getPosition();
      std::cout << "Position: " << pos.x << ", " << pos.y << ", " << pos.z << "\n";
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
      Config::option_normal_mapping_enabled =
          !Config::option_normal_mapping_enabled;
      if (Config::option_normal_mapping_enabled)
        std::cout << "Normal mapping ON \n";
      else
        std::cout << "Normal mapping OFF \n";
    }
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

        thiz->camera->changeOrientation(xoffset, yoffset);
      });
}

void Input::set_scroll_callback(GLFWwindow *window) {

  glfwSetScrollCallback(
      window, [](GLFWwindow *window, double /*xoffset*/, double yoffset) {
        auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
        thiz->camera->changeFovy(yoffset);
      });
}
