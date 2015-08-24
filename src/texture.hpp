#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>                 // for GLuint
#include <string> 


struct Texture {
	GLuint id;
	std::string type;
	std::string path;
};

#endif // TEXTURE_H
