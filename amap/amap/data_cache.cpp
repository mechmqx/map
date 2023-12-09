#include "data_cache.h"
#include <math.h>
#include <string>
#include <assert.h>


#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

cacheEle::cacheEle() {
	state = eEmpty;
}
cacheEle::~cacheEle() {}

void cacheEle::genVertex(tileId& id) {
	//GRID_SIZE
	float step = 90.0 / pow(2, id.level);
	float left = -180 + step * id.xidx;
	float bottom = -90 + step * id.yidx;

	float grid_step = step / (GRID_SIZE - 1.0);
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			this->vert[j * GRID_SIZE + i].x = left + grid_step * i;
			this->vert[j * GRID_SIZE + i].y = bottom + grid_step * j;
		}
	}
	this->state = eLoadImage;
}

void cacheEle::loadTexture(tileId& id) {
	std::string filename = "../../tools/rast/data/" + std::to_string(id.level) + "_" + std::to_string(id.xidx) + "_" + std::to_string(id.yidx) + ".png";
	int w, h, c;
	unsigned char *pixels = stbi_load(filename.c_str(), &w, &h, &c, 0);
	assert(w == IMG_SIZE);
	assert(h == IMG_SIZE);
	assert(c == IMG_CHN);
	memcpy(this->image, pixels, IMG_CHN * IMG_SIZE * IMG_SIZE); 
	this->state = eReady;
	stbi_image_free(pixels);
}

dataCache::dataCache() {
	_lru = new LRUCache(DATA_CACHE_SIZE);
}

dataCache::~dataCache() {}


int dataCache::getFreeCacheIndex(int key) {
	eIRUState state = eIRUNew;
	int ret = _lru->get(key, state);
	if (state != eIRUReady) {
	    cache[ret].state = eWaitLoading;
	}

	return ret;
}

void dataCache::freeCache(short idx) {
	delete _lru;
}