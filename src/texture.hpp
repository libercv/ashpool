#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>                 // for GLuint
#include <string> 

enum class TextureType { diffuse, specular };

struct Texture {
	GLuint id;
    //TextureType type;
    GLuint uniformId;
};

#endif // TEXTURE_H
