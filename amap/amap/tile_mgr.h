#pragma once
#include <vector>
#include "data_cache.h"
#include "renderer_cache.h"
#include "map_tile.h"

#include "lru_cache.h"

#define TILE_CACHE_SIZE (64)

class tileManager
{
public:
	tileManager();
	~tileManager();
	mapTile& getTile(int index);
	int addTile(tileId& id);

	std::vector<int> tileList;

	RendererEle& getRenderEle(short idx);
	void UpdateRenderEle(mapTile& tile);
	unsigned int tbo;
	unsigned int ibo;
	unsigned int gTBO;
	unsigned int gIBO;
	unsigned int gVBO;
	unsigned int gTexId;


private:
	LRUCache *_lru;
	dataCache *dataMgr;
	renderCache *renderMgr;
	int getFreeTile(int key);
	mapTile tileCache[TILE_CACHE_SIZE];
	unsigned long backgroundProcess();
};
