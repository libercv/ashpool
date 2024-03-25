/***************************************************
 * ShaderLoader
 *
 * Loads a GLSL OpenGL shader from a disk file and
 * compiles it.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <GL/glew.h> // for GLuint, GLchar
#include <string>    // for string

class ShaderLoader {
public:
  ShaderLoader(const std::string &path, GLuint shader_type);
  ~ShaderLoader();
  GLuint getId() { return mId; };

private:
  GLuint mId;
  std::string readFile(const std::string &path);
  void compile(const GLchar *, GLuint);
};

#endif // SHADERLOADER_H
