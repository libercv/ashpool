#include "configloader.hpp"
#include "renderengine.hpp"
#include "system.hpp"
#include "world.hpp"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cout << "Please provide configuration file\n";
    std::cout << "Example: ashpool sponza.ash\n";
    exit(-1);
  }

  ConfigLoader loadconfig(argv[1]);
  System system;
  World world;
  world.init();
  RenderEngine renderer(&world);

  system.setCamera(world.getCamera());

  while (!system.exitRequested()) {
    system.update();
    world.update();
    renderer.render();
  }

  return 0;
}
