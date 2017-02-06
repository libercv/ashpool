#include "renderengine.hpp"
#include "system.hpp"
#include "world.hpp"

int main() {
  System system;
  World world;
  RenderEngine renderer(&world);

  system.setCamera(world.getCamera());

  while (!system.exitRequested()) {
    system.update();
    world.update();
    renderer.render();
  }

  return 0;
}
