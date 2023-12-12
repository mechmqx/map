#pragma once
#include <string>

bool map_load_image(std::string imagefile, int& w, int& h, int& c, unsigned char** pixels);
int map_destroy_image(unsigned char** pixels);