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

/**
 * @brief Input::init
 * Object initialization
 * @param window indow which handles the event
 */
void Input::init(GLFWwindow *window) {
  /// Save pointer to «this» into «GL window»
  glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));
  /// Initialize array to zero
  std::memset(key_pressed, false, sizeof key_pressed);
  /// Set callbacks
  set_key_callback(window);
  set_mouse_callback(window);
  set_scroll_callback(window);
}

/**
 * @brief Input::move
 * Check cursor keys and move camera accordingly
 * @param delta_time Time in ms since last movement
 */
void Input::move(GLfloat delta_time) {
  if (key_pressed[GLFW_KEY_UP])
    camera->move(Camera::FORWARD, delta_time);
  if (key_pressed[GLFW_KEY_DOWN])
    camera->move(Camera::BACKWARD, delta_time);
  if (key_pressed[GLFW_KEY_LEFT])
    camera->move(Camera::LEFT, delta_time);
  if (key_pressed[GLFW_KEY_RIGHT])
    camera->move(Camera::RIGHT, delta_time);
}

//
/**
 * @brief Input::set_key_callback
 * Is called whenever a key is pressed/released via GLFW
 * Handles keys that have to react only once when pressed
 * @param window Window which handles the event
 */
void Input::set_key_callback(GLFWwindow *window) {
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int /*scancode*/,
                                int action, int /*mode*/) {
    auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
      switch (key) {
      case GLFW_KEY_ESCAPE:
      case GLFW_KEY_Q:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
      case GLFW_KEY_S:
        Config::option_shadows_enabled = !Config::option_shadows_enabled;
        std::cout << "Shadows "
                  << ((Config::option_shadows_enabled) ? "ON" : "OFF") << "\n";
        break;
      case GLFW_KEY_C:
        if (Config::rendering_method != Config::HYBRID_CPU) {
          Config::option_rendering_method_change_requested = true;
          Config::rendering_method = Config::HYBRID_CPU;
        }
        break;
      case GLFW_KEY_H:
        if (Config::rendering_method != Config::HYBRID) {
          if (Config::option_opencl_available) {
            Config::option_rendering_method_change_requested = true;
            Config::rendering_method = Config::HYBRID;
          } else
            std::cout << "OpenCL Shader not available\n";
        }
        break;
      case GLFW_KEY_D:
        if (Config::rendering_method != Config::DEFERRED) {
          Config::option_rendering_method_change_requested = true;
          Config::rendering_method = Config::DEFERRED;
        }
        break;
      case GLFW_KEY_P:
        Config::option_statistics_requested = true;
        break;
      case GLFW_KEY_R:
        Config::option_reset_statistics_requested = true;
        break;
      case GLFW_KEY_L:
        glm::vec3 pos = thiz->camera->getPosition();
        std::cout << "Camera location: " << pos.x << ", " << pos.y << ", "
                  << pos.z << "\n";
        break;
      }
    }

    /// Update array of key states
    if (key >= 0 && key < 1024) {
      if (action == GLFW_PRESS)
        thiz->key_pressed[key] = true;
      else if (action == GLFW_RELEASE)
        thiz->key_pressed[key] = false;
    }
  });
}

/**
 * @brief Input::set_mouse_callback
 * Sets mouse movement callback. Rotates the camera around
 * @param window Window which handles the event
 */
void Input::set_mouse_callback(GLFWwindow *window) {
  glfwSetCursorPosCallback(
      window, [](GLFWwindow *window, double xpos, double ypos) {

        auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
        /// Initialize mouse position in first call
        if (thiz->lastX == NOT_INITIALIZED) {
          thiz->lastX = xpos;
          thiz->lastY = ypos;
        }

        /// Calculate x and y offset
        double xoffset = xpos - thiz->lastX;
        /// Reversed since y-coordinates go from bottom to left
        double yoffset = thiz->lastY - ypos;

        /// Update las position
        thiz->lastX = xpos;
        thiz->lastY = ypos;

        /// Rotate the camera
        thiz->camera->rotate(
            static_cast<float>(xoffset) * Config::movement_sensitivity,
            static_cast<float>(yoffset) * Config::movement_sensitivity);
      });
}

/**
 * @brief Input::set_scroll_callback
 * Sets the callback used when mouse scroll wheel event is received
 * Updates camera's vertical field of view angle (fovy) and behaves
 * as a zoom
 * @param window Window which handles the event
 */
void Input::set_scroll_callback(GLFWwindow *window) {
  glfwSetScrollCallback(
      window, [](GLFWwindow *window, double /*xoffset*/, double yoffset) {
        auto thiz = reinterpret_cast<Input *>(glfwGetWindowUserPointer(window));
        thiz->camera->change_fovy(static_cast<float>(yoffset) *
                                  Config::movement_fovy_step);
      });
}
