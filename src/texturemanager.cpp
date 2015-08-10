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

	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(nullptr);
	//pointer to the image data
	BYTE* bits(nullptr);
	unsigned int width(0), height(0);

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filename.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return 0;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename.c_str());
	//if the image failed to load, return failure
	if(!dib)
		return 0;

	// Convert non-32 bit images
	if ( FreeImage_GetBPP( dib ) != 32 ) {
		FIBITMAP* oldDib = dib;
		dib = FreeImage_ConvertTo32Bits( oldDib );
		FreeImage_Unload( oldDib );
	}

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == nullptr) || (width == 0) || (height == 0))
		return 0;

	auto image = std::make_unique<GLbyte[]>(width * height * 4);
	for ( unsigned int i = 0; i < width; ++i ) {
		for ( unsigned int j = 0; j < height; ++j ) {
			unsigned int buf_index = ( j * width + i )*4;
			unsigned int buf_dst = (width * height * 4) - buf_index -4 ;
			image.get()[ buf_dst  + 0 ] = bits[buf_index + FI_RGBA_RED];
			image.get()[ buf_dst + 1 ] = bits[buf_index + FI_RGBA_GREEN];
			image.get()[ buf_dst + 2 ] = bits[buf_index + FI_RGBA_BLUE];
			image.get()[ buf_dst + 3 ] = bits[buf_index + FI_RGBA_ALPHA];
		}
	}

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.get());
	glGenerateMipmap(GL_TEXTURE_2D);	

	glBindTexture(GL_TEXTURE_2D, 0);
	std::cout << "Imported texture " << filename << std::endl;
	return textureID;
}


