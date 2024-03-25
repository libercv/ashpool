/***************************************************
 * TextureLoader
 *
 * Loads a texture from disk into memory and converts it
 * to RGBA format if it wasn't already.
 * Uses FreeImage library
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <GL/glew.h> // for GLbyte
#include <memory>    // for unique_ptr
#include <string>    // for string
struct FIBITMAP;

class TextureLoader {
public:
  explicit TextureLoader(const std::string &filename);
  ~TextureLoader();
  const GLbyte *getImagePtr() const;
  unsigned int getWidth() const { return width; }
  unsigned int getHeight() const { return height; }

private:
  FIBITMAP *dib{nullptr};
  unsigned int width{0};
  unsigned int height{0};
};

#endif // TEXTURELOADER_H
