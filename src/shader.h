#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


#include <GL/glew.h>

class Shader {
public:
	Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
	void use() const;
private:
	GLuint program;
	std::string readFile(const GLchar *name);
	GLuint compileShader(const GLchar* code, GLuint);
	void createProgram(GLuint, GLuint);
};


#endif // SHADER_H
