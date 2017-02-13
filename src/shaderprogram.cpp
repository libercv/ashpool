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
#include "shaderprogram.hpp"
#include "shaderloader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

ShaderProgram::ShaderProgram(const GLchar *vertexPath,
                             const GLchar *fragmentPath) {
  auto vertexShader = ShaderLoader(vertexPath, GL_VERTEX_SHADER);
  auto fragmentShader = ShaderLoader(fragmentPath, GL_FRAGMENT_SHADER);
  createProgram(vertexShader.getId(), fragmentShader.getId());
}

GLuint ShaderProgram::getUniformLocation(const std::string &name) const {
  return glGetUniformLocation(mProgram, name.data());
}

ShaderProgram::~ShaderProgram() {
  if (mProgram != 0)
    glDeleteProgram(mProgram);
}

void ShaderProgram::createProgram(GLuint vertexId, GLuint fragmentId) {
  mProgram = glCreateProgram();
  glAttachShader(mProgram, vertexId);
  glAttachShader(mProgram, fragmentId);
  glLinkProgram(mProgram);

  // Check the result
  GLint success = 0;
  glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetProgramInfoLog(mProgram, 512, NULL, infoLog);
    std::cerr << "Error linking OpenGL shaders \n"
              << infoLog << std::endl;
    std::exit(1);
  }
}

void ShaderProgram::use() const { glUseProgram(mProgram); }
