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
  RenderEngine(){};
  ~RenderEngine(){};
  void render(const World *w) { shader.render(w); }
  void clear(){};
};

#endif // RENDERENGINE_H
