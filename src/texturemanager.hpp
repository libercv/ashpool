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
  GLbyte *readTextureFromDisk(std::string path, unsigned int *widthp,
                              unsigned int *heightp, bool scale,
                              GLbyte **smallTexData) const;
  // GLuint load2dTexture(const std::string& path) const;
};

#endif // TEXTUREMANAGER_H
