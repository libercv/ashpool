#include "window.hpp"
#include "scene.hpp"
#include "cl_init.hpp"
#include "input.hpp"

int main() {

  Window window;
  Input input = window.createInput();
  Scene scene(&input);
  CL_Init opencl;

  while (!window.shouldClose()) {
    window.pollEvents();
    window.swapBuffers();
    scene.render();
  }

  return 0;
}
