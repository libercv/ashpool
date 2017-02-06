#ifndef RENDERENGINE_H
#define RENDERENGINE_H

#include "hybridshader.hpp" // for DeferredShader
#include "shader.hpp"
class World; // lines 10-10

class RenderEngine {
private:
  Shader<HybridShader> shader;

public:
  RenderEngine(const World *w) : shader(Shader<HybridShader>(w)){};
  ~RenderEngine(){};
  void render() { shader.render(); }
  void clear(){};
};

#endif // RENDERENGINE_H
