#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <GL/glew.h>          // for GLbyte, GLint, GLuint
#include <assimp/material.h>  // for aiMaterial (ptr only), aiTextureType
#include <string>             // for string
#include <vector>             // for vector
#include "mesh.hpp"           // for Texture

class TextureManager {
	public:
		/*  Functions   */
		// Constructor, expects a filepath to a 3D model.

		static TextureManager& get(){
			static TextureManager instance;
			return instance;
		}

		// Checks all material textures of a given type and loads the textures if they're not loaded yet.
		// The required info is returned as a Texture struct.
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const std::string& directory);
	private:
		TextureManager() {};
		std::vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		GLint TextureFromFile(const std::string& filename); 
		GLbyte* readTextureFromDisk(std::string path, unsigned int* widthp, unsigned int* heightp, bool scale, GLbyte **smallTexData) const;
		GLuint load2dTexture(const std::string& path);

};



#endif // TEXTUREMANAGER_H
