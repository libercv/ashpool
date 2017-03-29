/***************************************************
 * ShaderProgram
 *
 * It stores a Shader program and allows to use it.
 * During its initialization loads a vertex and a fragment
 * shaders (using ShaderLoader) and creates the program,
 * linking them.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h> // for GLuint, GLchar
#include <string>    // for string

class ShaderProgram {
public:
  ShaderProgram(const GLchar *vertexSourcePath,
                const GLchar *fragmentSourcePath);
  ~ShaderProgram();
  void use() const;
  GLuint getUniformLocation(const std::string &name) const;

private:
  GLuint mProgram;
  void createProgram(GLuint, GLuint);
};

#endif // SHADERPROGRAM_H
