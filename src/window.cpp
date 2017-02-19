/***************************************************
 * Window
 *
 * Destop window initialization, event polling and swaping
 * methods. Uses GLFW3 and GLEW
 *
 * 2017 - Liberto Camús
 * **************************************************/
#include "window.hpp"
#include "input.hpp"
#include <functional>

Window::Window() {
  // Init GLFW
  if (!glfwInit()) {
    std::cerr << "GLFW Error initializing" << std::endl;
    std::exit(1);
  }

  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "ASHPOOL", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "GLFW Error creating window" << std::endl;
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0); // 0-> no sync, more than 60fps possible

  // Options
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  GLenum glewres = glewInit();
  if (GLEW_OK != glewres) {
    std::cerr << "Error: " << glewGetErrorString(glewres) << std::endl;
    std::exit(1);
  }

  // Define the viewport dimensions
  glViewport(0, 0, WIDTH, HEIGHT);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  glEnable(GL_DEPTH_TEST);
}

Input Window::createInput() { return Input(window); }

Window::~Window() { glfwTerminate(); }

bool Window::shouldClose() { return glfwWindowShouldClose(window); }

void Window::pollEvents() { glfwPollEvents(); }

void Window::swapBuffers() { glfwSwapBuffers(window); }
