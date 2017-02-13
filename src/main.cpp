/***************************************************
 * Main
 *
 * Main method and loop
 * 
 * 2017 - Liberto Camús
 * **************************************************/
#include "configloader.hpp"
#include "renderengine.hpp"
#include "system.hpp"
#include "world.hpp"

int main(int argc, char *argv[]) {
  // Check command line interface arguments
  if (argc < 2) {
    std::cout << "Please provide configuration file\n";
    std::cout << "Example: ashpool sponza.ash\n";
    exit(-1);
  }

  // Load configuration and scene description
  ConfigLoader loadconfig(argv[1]);
  
  // Initialize engine objects
  System system;
  World world;
  world.init();
  RenderEngine renderer(&world);
  system.setCamera(world.getCamera());

  // Main loop
  while (!system.exitRequested()) {
    // Update timers, keyboard events...
    system.update();
    // Update geometry and scene options
    world.update();
    // Render the scene
    renderer.render();
  }

  return 0;
}
