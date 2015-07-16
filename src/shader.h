#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

#include <GL/glew.h>

class Shader {
public:
	Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
	void use();
private:
	GLuint program;
	string readFile(const GLchar *name);
	GLuint compileShader(const GLchar* code, GLuint);
	void createProgram(GLuint, GLuint);
};


#endif // SHADER_H
