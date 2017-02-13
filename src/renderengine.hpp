/***************************************************
 * RenderEngine
 *
 * Instantiates a "RenderingMethod" template to render the
 * scene using one of the available methods.
 * Currently there are two possibilities: Deferred 
 * Shader (full OpenGL) or Hybrid Shader (OpenGL+OpenCL)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include "hybridshader.hpp"
#include "renderingmethod.hpp"
class World;

class RenderEngine {
private:
  RenderingMethod<HybridShader> shader;

public:
  RenderEngine(World *w) : shader(RenderingMethod<HybridShader>(w)){};
  ~RenderEngine(){};
  void render() { shader.render(); }
  void clear(){};
};

#endif // RENDERENGINE_H
