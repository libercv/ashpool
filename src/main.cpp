#include "system.hpp"
#include "world.hpp"
#include "renderengine.hpp"


int main() {
  System system;
  World world;
  RenderEngine renderer(&world);

  system.setCamera(world.getCamera());

  while (!system.exitRequested()) {
    system.update();
    renderer.render();
  }

  return 0;
}
