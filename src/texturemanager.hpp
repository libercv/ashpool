/***************************************************
 * TextureManager
 *
 * Loads textures corresponding to ASSIMP materials from disk,
 * using "TextureLoader", uploads it to OpenGL GPU memory and
 * returns a "Texture" object with its type and OpenGL id.
 * Stores internally already loaded textures to avoid loading a
 * texture more than once.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "mesh.hpp"          // for Texture
#include <GL/glew.h>         // for GLbyte, GLint, GLuint
#include <assimp/material.h> // for aiMaterial (ptr only), aiTextureType
#include <string>            // for string
#include <vector>            // for vector

class TextureManager {
public:
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            const std::string &directory);

private:
  // Struct for internal use to avoid loading duplicated textures
  struct TextureLoaded {
    GLuint id;
    std::string path;
  };

  std::vector<TextureLoaded> textures_loaded;
  GLint TextureFromFile(const std::string &filename) const;
};

#endif // TEXTUREMANAGER_H
