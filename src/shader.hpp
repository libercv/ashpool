#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>  // for GLuint, GLchar
#include <string>     // for string

class Shader {
public:
	Shader(const std::string &path, GLuint shader_type);
	~Shader();
	GLuint getId() { return mId; };
private:
	GLuint mId;
	std::string readFile(const std::string &path);
	void compile(const GLchar*, GLuint);
};


#endif // SHADER_H
