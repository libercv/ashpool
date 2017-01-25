#include "system.hpp"
#include "world.hpp"
#include "renderengine.hpp"


int main() {
  System system;
  World world;
  RenderEngine renderer(&world);

  system.setCamera(world.getCamera());

  float luz_rad=0.0f;
  while (!system.exitRequested()) {
    system.update();
    
    
    float radio=4.2f;
    luz_rad=luz_rad+0.02f;
    if (luz_rad>2*3.1415) 
        luz_rad-=2*3.1415;
        
    world.setPointLight(0, glm::vec3{radio*cos(luz_rad), 1.5f, radio*sin(luz_rad)});
    world.setPointLight(1, glm::vec3{(radio+0.15f)*cos(luz_rad), 1.5f, radio*sin(luz_rad)});
    world.setPointLight(2, glm::vec3{(radio+0.15f)*cos(luz_rad), 1.5f, (radio+0.15f)*sin(luz_rad)});
    world.setPointLight(3, glm::vec3{radio*cos(luz_rad), 1.5f, (radio+0.15f)*sin(luz_rad)});
    
    renderer.render();
  }

  return 0;
}
