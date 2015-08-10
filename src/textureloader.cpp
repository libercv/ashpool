// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <map>
#include <vector>

#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <FreeImage.h>

#include "textureloader.h"

TextureLoader::TextureLoader(const std::string& filename) {
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image data
	BYTE* bits(nullptr);

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename.c_str(), 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filename.c_str());
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return ;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename.c_str());
	//if the image failed to load, return failure
	if(!dib)
		return ;

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
		return ;

	img_ptr = std::make_unique<GLbyte[]>(width * height * 4);

	unsigned int src_offset = 0;
	unsigned int dst_offset = (width * height * 4) -4;
	for ( unsigned int i = 0; i < width * height ; ++i ) {
		img_ptr.get()[ dst_offset + 0 ] = bits[src_offset + FI_RGBA_RED];
		img_ptr.get()[ dst_offset + 1 ] = bits[src_offset + FI_RGBA_GREEN];
		img_ptr.get()[ dst_offset + 2 ] = bits[src_offset + FI_RGBA_BLUE];
		img_ptr.get()[ dst_offset + 3 ] = bits[src_offset + FI_RGBA_ALPHA];
		src_offset += 4;
		dst_offset -= 4;
	}

}


TextureLoader::~TextureLoader() {
	//Free FreeImage's copy of the data
	if(dib)
		FreeImage_Unload(dib);
}


