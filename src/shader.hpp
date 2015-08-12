#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>  // for GLuint, GLchar
#include <string>     // for string

class Shader {
public:
	Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
	void use() const;
	GLuint getUniformLocation(const std::string& name) const;
private:
	GLuint program;
	std::string readFile(const GLchar *name);
	GLuint compileShader(const GLchar* code, GLuint);
	void createProgram(GLuint, GLuint);
};


#endif // SHADER_H
