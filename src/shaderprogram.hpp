#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <GL/glew.h>  // for GLuint, GLchar
#include <string>     // for string

class ShaderProgram {
public:
	ShaderProgram(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
	~ShaderProgram();
	void use() const;
	GLuint getUniformLocation(const std::string& name) const;
private:
	GLuint mProgram;
	void createProgram(GLuint, GLuint);
};


#endif // SHADERMANAGER_H
