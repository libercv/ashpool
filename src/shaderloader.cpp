#include "shaderloader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

ShaderLoader::ShaderLoader(const std::string &path, GLuint shader_type)
    : mId{0} {
  std::string vertexCodeStr = readFile(path);
  const GLchar *code = vertexCodeStr.c_str();
  compile(code, shader_type);
}

ShaderLoader::~ShaderLoader() {
  if (mId != 0)
    glDeleteShader(mId);
}

void ShaderLoader::compile(const GLchar *code, GLuint shader_type) {
  // Compile shader
  mId = glCreateShader(shader_type);
  glShaderSource(mId, 1, &code, NULL);
  glCompileShader(mId);

  // Check the result of compilation
  GLint success;
  glGetShaderiv(mId, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(mId, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog;
    std::exit(1);
  }
}

std::string ShaderLoader::readFile(const std::string &path) {
  std::string code{""};
  try {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path);
    std::stringstream fStream;
    fStream << file.rdbuf();
    file.close();
    code = fStream.str();
  } catch (std::ifstream::failure &e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
    std::cerr << "File: " << path << std::endl;
    std::exit(1);
  }
  return code;
}
