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
  if (fif == FIF_UNKNOWN) {
    std::cout << "Unknown image type. Error loading texture " << filename
              << "\n";
    return;
  }

  // check that the plugin has reading capabilities and load the file
  if (FreeImage_FIFSupportsReading(fif))
    dib = FreeImage_Load(fif, filename.c_str());
  // if the image failed to load, return failure
  if (!dib) {
    std::cout << "Error loading texture " << filename << "\n";
    return;
  }

  // Convert non-32 bit images
  if (FreeImage_GetBPP(dib) != 32) {
    FIBITMAP *oldDib = dib;
    dib = FreeImage_ConvertTo32Bits(oldDib);
    FreeImage_Unload(oldDib);
  }

  BYTE *bits = FreeImage_GetBits(dib);
  width = FreeImage_GetWidth(dib);
  height = FreeImage_GetHeight(dib);

  // Test for errors
  if ((bits == nullptr) || (width == 0) || (height == 0))
    std::cout << "Error loading texture " << filename << "\n";
}

const GLbyte *TextureLoader::getImagePtr() const {
  return reinterpret_cast<const signed char *>(FreeImage_GetBits(dib));
}

TextureLoader::~TextureLoader() {
  if (dib)
    FreeImage_Unload(dib);
}
