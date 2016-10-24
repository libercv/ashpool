#include "system.hpp"
#include "world.hpp"
#include "renderengine.hpp"


int main() {
  System system;
  World world;
  RenderEngine renderer;

  system.setCamera(world.getCamera());

  while (!system.exitRequested()) {
    system.update();
    renderer.render(&world);
  }

  return 0;
}
