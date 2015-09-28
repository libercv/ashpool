#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <GL/glew.h>          // for GLbyte, GLint, GLuint
#include <assimp/material.h>  // for aiMaterial (ptr only), aiTextureType
#include <string>             // for string
#include <vector>             // for vector
#include "mesh.hpp"           // for Texture

class TextureManager {
	public:
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
							  const std::string& directory);
	private:
		// Struct for internal use to avoid loading duplicated textures
		struct TextureLoaded {
			GLuint id;
			std::string path;
		};

		std::vector<TextureLoaded> textures_loaded;	
		GLint TextureFromFile(const std::string& filename); 
		GLbyte* readTextureFromDisk(std::string path, unsigned int* widthp, 
					    unsigned int* heightp, bool scale, GLbyte **smallTexData) const;
		GLuint load2dTexture(const std::string& path);

};

#endif // TEXTUREMANAGER_H
