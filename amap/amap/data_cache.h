#pragma once

#include "config.h"
#include "map_tile.h"
#include "MAP_Math.h"
#include "lru_cache.h"
#include "tile_id.h"
#include <Windows.h>

#define DATA_CACHE_SIZE (128)

typedef enum eCacheState{
	eEmpty,
	eWaitLoading,
	eLoadVertex,
	eLoadImage,
	eReady
}eCacheState;


class cacheEle {
public:
	Vec2f vert[TILE_V_NUM];
	char image[IMG_CHN* IMG_SIZE* IMG_SIZE];
	eCacheState state;   // 0: empty	1:load vert	2:load image	3:ready
	void lockCache();
	void unlockCache();
	void genVertex(tileId& id);
	void loadTexture(tileId& id);
	cacheEle();
	~cacheEle();
private:
	HANDLE _mutex;
};

class dataCache {
public:
	dataCache();
	~dataCache();
	void updateCacheIndex(tileId& id);
	int getFreeCacheIndex(tileId& id, tileId& oldid);

	cacheEle& getElement(short idx);

private:
	cacheEle _cache[DATA_CACHE_SIZE];
	LRUCache* _lru;
};