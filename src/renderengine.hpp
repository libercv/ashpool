#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include "cl_init.hpp"
#include "shader.hpp"
#include "deferredshader.hpp"  // for DeferredShader
class World;  // lines 10-10

class RenderEngine {
private:
  Shader<DeferredShader> shader;
  CL_Init opencl;

public:
  RenderEngine(const World *w) : shader{Shader<DeferredShader>(w)} {};
  ~RenderEngine(){};
  void render() { shader.render(); }
  void clear(){};
};

#endif // RENDERENGINE_H
