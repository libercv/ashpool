/***************************************************
 * RenderEngine
 *
 * Abstract class intended to be the interface every
 * rendering method should inherit from.
 * Currently there are three possibilities: Deferred
 * Shader (full OpenGL), Hybrid Shader (OpenGL+OpenCL)
 * and CPU Hybrid Shader (OpenGL+OpenMP)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef RENDERENGINE_H
#define RENDERENGINE_H

class RenderEngine {
public:
  virtual void render() = 0;
  virtual ~RenderEngine() {}
};

#endif // RENDERENGINE_H
