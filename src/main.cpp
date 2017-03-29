/***************************************************
 * Main
 *
 * Main method and loop
 *
 * 2017 - Liberto Camús
 * **************************************************/
#include "configloader.hpp"
#include "deferredshader.hpp"
#include "hybridshader.hpp"
#include "hybridshadercpu.hpp"
#include "renderengine.hpp"
#include "system.hpp"
#include "world.hpp"

void parseCLI(int argc, char *argv[]) {

  for (int i = 1; i < (argc - 1); i++) {
    std::string option{argv[i]};
    if (option.compare("--cpu") == 0)
      Config::rendering_method = Config::HYBRID_CPU;
    else if (option.compare("--deferred") == 0)
      Config::rendering_method = Config::DEFERRED;
    else if (option.compare("--hybrid") == 0)
      Config::rendering_method = Config::HYBRID;
    else if (option.compare("--noshadows") == 0)
      Config::option_shadows_enabled = false;
    else if (option.compare("--nocapturemouse") == 0)
      Config::option_no_capture_mouse = true;
  }
}

std::unique_ptr<RenderEngine> createRenderEngine(World &world) {
  std::unique_ptr<RenderEngine> ren;
  switch (Config::rendering_method) {
  case Config::HYBRID:
    ren = std::make_unique<HybridShader>(&world);
    std::cout << "Creating Hybrid Shader\n";
    break;
  case Config::HYBRID_CPU:
    ren = std::make_unique<HybridShaderCPU>(&world);
    std::cout << "Creating Hybrid CPU Shader\n";
    break;
  default:
    ren = std::make_unique<DeferredShader>(&world);
    std::cout << "Creating Deferred Shader\n";
  }
  Config::option_rendering_method_change_requested = false;
  return ren;
}

int main(int argc, char *argv[]) {
  // Check command line interface arguments
  if (argc < 2) {
    std::cout << "Usage: ashpool <options> configuration_file\n";
    std::cout << "Example: ashpool --cpu --noshadows sponza.ash\n\n";
    std::cout << "Options:\n";
    std::cout << "  --cpu: Use Hybrid Shader on CPU\n";
    std::cout << "  --hybrid: Use Hybrid Shader with OpenCL\n";
    std::cout << "  --deferred: User Deferred Shader\n";
    std::cout
        << "  --noshadows: Do not calculate shadows with Hybrid shaders\n";
    std::cout << "  --nocapturemouse: Do not capture mouse pointer. \n\n";
    exit(-1);
  }

  // Load configuration and scene description
  ConfigLoader loadconfig(argv[argc - 1]);
  parseCLI(argc, argv);

  // Initialize engine objects
  System system;
  World world;
  world.init();
  std::unique_ptr<RenderEngine> renderer = createRenderEngine(world);
  system.setCamera(world.getCamera());

  // Main loop
  while (!system.exitRequested()) {
    // Update timers, keyboard events...
    system.update();
    // Update geometry and scene options
    world.update();
    // Render the scene
    renderer->render();
    if (Config::option_rendering_method_change_requested) {
      system.printStatistics();
      renderer = createRenderEngine(world);
      system.resetStatistics();
    }
  }
  system.printStatistics();
  return 0;
}
