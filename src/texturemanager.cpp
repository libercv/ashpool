#include "texturemanager.hpp"
#include "textureloader.hpp" // for TextureLoader
#include "vertex.hpp"
#include <GL/glew.h> // for GL_TEXTURE_2D, glTexParameteri, etc
#include <algorithm>
#include <assimp/material.inl> // for aiMaterial::GetTexture, etc
#include <assimp/types.h>      // for aiString
#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <iostream>            // for basic_ostream, operator<<, cout, etc
#include <string>              // for allocator, string, operator+, etc
#include <vector>              // for vector

std::vector<Texture>
TextureManager::loadMaterialTextures(aiMaterial *mat, aiTextureType aiType,
                                     const std::string &directory) {

  std::vector<Texture> textures;

  for (GLuint i = 0; i < mat->GetTextureCount(aiType); i++) {
    // Get texture name
    aiString str;
    mat->GetTexture(aiType, i, &str);
    auto path = std::string(str.C_Str());

    // Find out if we have already loaded it.
    auto found =
        std::find_if(begin(textures_loaded), end(textures_loaded),
                     [&path](TextureLoaded &t) { return t.path == path; });

    if (found != end(textures_loaded)) {
      // The texture was already loaded
      textures.push_back(Texture{(*found).id, 0});
    } else {
      // Load a new texture
      TextureLoaded texture;
      texture.id = TextureFromFile(directory + "/" + path);
      texture.path = path;
      textures.push_back(Texture{texture.id, 0});
      textures_loaded.push_back(std::move(texture));
    }
  }

  return textures;
}

GLint TextureManager::TextureFromFile(const std::string &filename) const {

  // Generate texture ID and load texture data
  GLuint textureID = 0;
  glGenTextures(1, &textureID);

  // Load the image
  TextureLoader image{filename};
  if (image.getImagePtr() == nullptr) {
    std::cout << "Failed loading " << filename << std::endl;
    return -1;
  }

  // Assign texture to ID
  glBindTexture(GL_TEXTURE_2D, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, image.getImagePtr());
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);
  std::cout << "Imported texture " << filename << std::endl;

  return textureID;
}
