#include "image_load.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

bool map_load_image(std::string imagefile, int& w, int& h, int& c, unsigned char** pixels) {
	*pixels = stbi_load(imagefile.c_str(), &w, &h, &c, 0);
	if (*pixels != 0) {
		return true;
	}
	else {
		return false;
	}
}
int map_destroy_image(unsigned char** pixels) {
	stbi_image_free(*pixels);
	return 1;
}