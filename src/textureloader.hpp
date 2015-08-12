#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <GL/glew.h>  // for GLbyte
#include <memory>     // for unique_ptr
#include <string>     // for string
struct FIBITMAP;

class TextureLoader {
public:
	explicit TextureLoader( const std::string &filename);
	~TextureLoader();
	const GLbyte* getImagePtr() const { return img_ptr.get(); };
	unsigned int getWidth() { return width; };
	unsigned int getHeight() { return height; };
private:
	std::unique_ptr<GLbyte[]> img_ptr;
	FIBITMAP *dib{nullptr};
	unsigned int width{0};
	unsigned int height{0};
};


#endif // TEXTURELOADER_H
