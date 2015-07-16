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
	Shader();
	void use();
	bool load(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
private:
	GLuint program;
};


#endif // SHADER_H
