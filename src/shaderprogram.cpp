#include "shaderprogram.hpp"
#include "shader.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

ShaderProgram::ShaderProgram(const GLchar *vertexPath,
                             const GLchar *fragmentPath) {
  auto vertexShader = Shader(vertexPath, GL_VERTEX_SHADER);
  auto fragmentShader = Shader(fragmentPath, GL_FRAGMENT_SHADER);
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
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog
              << std::endl;
    std::exit(1);
  }
}

void ShaderProgram::use() const { glUseProgram(mProgram); }
