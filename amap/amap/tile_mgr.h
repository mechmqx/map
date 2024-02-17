#pragma once
#include <vector>
#include <queue>
#include "data_cache.h"
#include "renderer_cache.h"
#include "map_tile.h"
#include "type_define.h"
#include "lru_cache.h"
#include "cam_mgr.h"
#include <unordered_map>

#define TILE_CACHE_SIZE (128)
#define ROOT_TILE_CACHE_SIZE (8)

class tileManager
{
	bool checkTileVisible(mapTile& tile);
public:
	tileManager();
	tileManager(camManager* camMgr);
	~tileManager();
	mapTile& getTile(int index);
	void updateTileList(sCtrlParam& param);
	void uploadTile();
	RendererEle& getRenderEle(short idx);

	std::vector<int> tileList;        // tile list to render
	std::queue<int> loadList;         // tile queue to load data 
	std::queue<int> uploadList;       // tile queue to upload data to GPU

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
	camManager* camMgr;
	mapTile tileCache[TILE_CACHE_SIZE+ ROOT_TILE_CACHE_SIZE];
	mapTile* root[8];

	int getFreeTile(tileId& id);
	int getTileIndex(tileId& id);
	void updateTileIndex(tileId& id);
	unsigned long backgroundProcess();
	void UpdateRenderEle(mapTile& tile);

	int getDataIndex(mapTile& tile);	
    int getRenderIndex(mapTile& tile);

};
