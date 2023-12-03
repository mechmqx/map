#pragma once
#include <vector>
#include "data_cache.h"
#include "renderer_cache.h"
#include "map_tile.h"

#define TILE_CACHE_SIZE (64)

class tileManager
{
public:
	tileManager();
	~tileManager();
	mapTile* getTile(short index);
	int addTile(tileId& id);

	std::vector<short> tileList;

	RendererEle& getRederEle(short idx);
	unsigned int tbo;
	unsigned int ibo;


private:
	dataCache *dataMgr;
	renderCache *renderMgr;
	int getFreeTile();
	mapTile tileCache[TILE_CACHE_SIZE];
};
