#ifndef SHADER_H
#define SHADER_H

#include "shaderprogram.hpp"

class World;

template <typename T> class Shader {
private:
  T elem;

public:
  Shader() {}
  ~Shader() {}
  void render(const World *w) { elem.render(w); }
  const ShaderProgram &getModelShader() { return elem.getModelShader(); }
};



#endif // SHADER_H
