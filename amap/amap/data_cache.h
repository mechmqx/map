#pragma once

#include "config.h"
#include "type_define.h"
#include "map_tile.h"

#define DATA_CACHE_SIZE (128)

enum eCacheState{
	eEmpty,
	eWaitLoading,
	eLoadVertex,
	eLoadImage,
	eReady
};


typedef struct _cacheEle {
	vertex2f vert[TILE_V_NUM];
	char image[IMG_CHN* IMG_SIZE* IMG_SIZE];
	eCacheState state;   // 0: empty	1:load vert	2:load image	3:ready
	void genVertex(tileId& id);
	void loadTexture(tileId& id);
}cacheEle;

class dataCache {
public:
	dataCache();
	~dataCache();
	short getFreeCacheIndex();
	void freeCache(short idx);
	cacheEle cache[DATA_CACHE_SIZE];
private:
};