/***************************************************
 * RenderingMethod
 *
 * Template to be able to use one of the available methods.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef RENDERINGMETHOD_H
#define RENDERINGMETHOD_H

#include "shaderprogram.hpp"

class World;

template <typename T> class RenderingMethod {
private:
  T elem;

public:
  RenderingMethod(World *w) : elem(w) {}
  ~RenderingMethod() {}
  void render() { elem.render(); }
  const ShaderProgram &getModelShader() { return elem.getModelShader(); }
};

#endif // RENDERINGMETHOD_H
