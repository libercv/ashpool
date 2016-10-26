#ifndef SHADER_H
#define SHADER_H

#include "shaderprogram.hpp"

class World;

template <typename T> class Shader {
private:
  T elem;

public:
  Shader(const World *w) : elem{w} {}
  ~Shader() {}
  void render() { elem.render(); }
  const ShaderProgram &getModelShader() { return elem.getModelShader(); }
};



#endif // SHADER_H
