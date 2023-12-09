#pragma once

#include "config.h"
#include "map_tile.h"
#include "MAP_Math.h"
#include "lru_cache.h"

#define DATA_CACHE_SIZE (128)

enum eCacheState{
	eEmpty,
	eWaitLoading,
	eLoadVertex,
	eLoadImage,
	eReady
};


class cacheEle {
public:
	Vec2f vert[TILE_V_NUM];
	char image[IMG_CHN* IMG_SIZE* IMG_SIZE];
	eCacheState state;   // 0: empty	1:load vert	2:load image	3:ready
	void genVertex(tileId& id);
	void loadTexture(tileId& id);
	cacheEle();
	~cacheEle();
};

class dataCache {
public:
	dataCache();
	~dataCache();
	int getFreeCacheIndex(int key);
	void freeCache(short idx);
	cacheEle cache[DATA_CACHE_SIZE];
private:
	LRUCache* _lru;
};