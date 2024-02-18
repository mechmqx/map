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
public:
	tileManager(camManager* camMgr);
	~tileManager();
	mapTile& getTile(int index);
	void updateTileList(sCtrlParam& param);
	void uploadTile();
	RendererEle& getRenderElement(short idx);

	std::vector<int> tileList;        // tile list to render
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

	bool _checkTileVisible(mapTile& tile);
	void _init();
	int _getFreeTile(tileId& id);
	int _getTileIndex(tileId& id);
	void _updateTileIndex(tileId& id);
	unsigned long _backgroundProcess();
	void _updateRenderElement(mapTile& tile);

	int _getDataIndex(mapTile& tile);	
    int _getRenderIndex(mapTile& tile);

};
