/***************************************************
 * TextureLoader
 *
 * Loads a texture from disk into memory and converts it
 * to RGBA format if it wasn't already.
 * Uses FreeImage library
 *
 * 2017 - Liberto Camús
 * **************************************************/
#include "textureloader.hpp"
#include <FreeImage.h> // for BYTE, FreeImage_Unload, etc
#include <GL/glew.h>   // for GLbyte
#include <iostream>
#include <memory> // for unique_ptr, make_unique
#include <string> // for string

TextureLoader::TextureLoader(const std::string &filename) {
  // Check the file signature and deduce its format
  auto fif = FreeImage_GetFileType(filename.c_str(), 0);
  if (fif == FIF_UNKNOWN)
    fif = FreeImage_GetFIFFromFilename(filename.c_str());
  if (fif == FIF_UNKNOWN)
    return;

  // check that the plugin has reading capabilities and load the file
  if (FreeImage_FIFSupportsReading(fif))
    dib = FreeImage_Load(fif, filename.c_str());
  // if the image failed to load, return failure
  if (!dib)
    return;

  // Convert non-32 bit images
  if (FreeImage_GetBPP(dib) != 32) {
    FIBITMAP *oldDib = dib;
    dib = FreeImage_ConvertTo32Bits(oldDib);
    FreeImage_Unload(oldDib);
  }

  // retrieve the image data
  BYTE *bits = FreeImage_GetBits(dib);
  // get the image width and height
  width = FreeImage_GetWidth(dib);
  height = FreeImage_GetHeight(dib);
  // if this somehow one of these failed (they shouldn't), return failure
  if ((bits == nullptr) || (width == 0) || (height == 0))
    return;

  // Texture is vertically flipped. Fix it  
  constexpr int bytes_per_pixel = 4;
  img_ptr = std::make_unique<GLbyte[]>(width * height * bytes_per_pixel);

  //auto line_length = width * bytes_per_pixel;
  //unsigned int src_offset = (line_length * (height - 1));
  unsigned int src_offset = 0;
  auto dst_ptr = img_ptr.get();
  for (unsigned int i = 0; i < height; i++) {
    for (unsigned int j = 0; j < width; ++j) {
      *dst_ptr = bits[src_offset + FI_RGBA_RED];
      dst_ptr++;
      *dst_ptr = bits[src_offset + FI_RGBA_GREEN];
      dst_ptr++;
      *dst_ptr = bits[src_offset + FI_RGBA_BLUE];
      dst_ptr++;
      *dst_ptr = bits[src_offset + FI_RGBA_ALPHA];
      dst_ptr++;
      src_offset += bytes_per_pixel;
    }
    //src_offset -= line_length * 2;
  }
}

const GLbyte *TextureLoader::getImagePtr() const {
  //return FreeImage_GetBits(dib);
  return img_ptr.get(); 
}

TextureLoader::~TextureLoader() {
  // Free FreeImage's copy of the data
  if (dib)
    FreeImage_Unload(dib);
}
