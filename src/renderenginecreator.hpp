/***************************************************
 * RenderEngineCreator
 *
 * Factory Method to create the a rendering engine  
 * based on Config::rendering_method enum.
 * Currently there are three possibilities: Deferred
 * Shader (full OpenGL), Hybrid Shader (OpenGL+OpenCL)
 * and CPU Hybrid Shader (OpenGL+OpenMP)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef RENDERENGINECREATOR_H
#define RENDERENGINECREATOR_H

#include <memory>
#include <iostream>
#include "renderengine.hpp"
#include "config.hpp"
#include "hybridshader.hpp"
#include "hybridshadercpu.hpp"
#include "deferredshader.hpp"
#include "system.hpp"
class World;


class RenderEngineCreator {
public:
  static std::unique_ptr<RenderEngine> create(World &world, System &system) {
    std::unique_ptr<RenderEngine> ren;
    switch (Config::rendering_method) {
    case Config::HYBRID:      
      if (!Config::option_opencl_available) {
          std::cout << "OpenCL not available\n";
          exit(1);
      }

      ren = std::make_unique<HybridShader>(&world, system.clmanager.get());
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
};

#endif // RENDERENGINECREATOR_H
