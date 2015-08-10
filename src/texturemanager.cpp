// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FreeImage.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "texturemanager.h"
#include "textureloader.h"


std::vector<Texture> TextureManager::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const std::string& directory) {
	std::vector<Texture> textures;
	for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for(GLuint j = 0; j < textures_loaded.size(); j++) {
			if(textures_loaded[j].path == str) {
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if(!skip) {   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(directory + "/" + str.C_Str());
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}


GLint TextureManager::TextureFromFile(const std::string& filename) {
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
		
	// Load the image
	TextureLoader image{filename};
	if (image.getImagePtr()==nullptr) {
		std::cout << "Failed loading " << filename << std::endl;
		return -1;
	}

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
			image.getWidth(), image.getHeight(), 0, GL_RGBA, 
			GL_UNSIGNED_BYTE, image.getImagePtr());
	glGenerateMipmap(GL_TEXTURE_2D);	

	glBindTexture(GL_TEXTURE_2D, 0);
	std::cout << "Imported texture " << filename << std::endl;
	return textureID;
}


