#ifndef SHADER_H
#define SHADER_H

#include "model.hpp"
#include "shaderprogram.hpp"
#include <vector>

class Camera;

template <typename T> class Shader {
private:
  T elem;

public:
  Shader() {}
  ~Shader() {}
  void render(const Camera *c, const std::vector<Model> &m) {
    elem.render(c, m);
  }
  const ShaderProgram &getModelShader() { return elem.getModelShader(); }
};



#endif // SHADER_H
