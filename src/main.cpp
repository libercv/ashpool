#include "window.hpp"
#include "scene.hpp"
#include "cl_init.hpp"

int main() {

  Window window;
  Scene scene;
  CL_Init opencl;

  while (!window.shouldClose()) {
    window.pollEvents();
    window.swapBuffers();
    scene.render();
  }

  return 0;
}
