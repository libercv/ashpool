/***************************************************
 * Texture
 *
 * Contains OpenGL texture id and its type 
 * (diffuse, specular, normal...)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h> // for GLuint

enum class TextureType { diffuse, specular, normal };

struct Texture {
  GLuint id;
  GLuint uniformId;
};

#endif // TEXTURE_H
