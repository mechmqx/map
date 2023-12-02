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
	int getFreeTile();

	dataCache *dataMgr;
	renderCache *renderMgr;

	std::vector<short> tileList;


private:
	mapTile tileCache[TILE_CACHE_SIZE];
};
