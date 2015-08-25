#include "texturemanager.hpp"
#include <GL/glew.h>            // for GL_TEXTURE_2D, glTexParameteri, etc
#include <assimp/types.h>       // for aiString
#include <ext/alloc_traits.h>   // for __alloc_traits<>::value_type
#include <assimp/material.inl>  // for aiMaterial::GetTexture, etc
#include <iostream>             // for basic_ostream, operator<<, cout, etc
#include <string>               // for allocator, string, operator+, etc
#include <vector>               // for vector
#include "textureloader.hpp"    // for TextureLoader
#include "vertex.hpp"

std::vector<Texture> TextureManager::loadMaterialTextures(aiMaterial* mat,
             aiTextureType aiType, TextureType texType, const std::string& directory) {
    std::vector<Texture> textures;
    for(GLuint i = 0; i < mat->GetTextureCount(aiType); i++)
	{
		aiString str;
        mat->GetTexture(aiType, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for(GLuint j = 0; j < textures_loaded.size(); j++) {
			if(textures_loaded[j].path == std::string(str.C_Str())) {
                textures.push_back(Texture{textures_loaded[j].id, 0});
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if(!skip) {   // If texture hasn't been loaded already, load it
			TextureLoaded texture;
			texture.id = TextureFromFile(directory + "/" + str.C_Str());
            //texture.type = texType;
			texture.path = std::string(str.C_Str());
			this->textures_loaded.push_back(texture);  
            textures.push_back(Texture{texture.id, 0});
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


