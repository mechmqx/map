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
	tileManager(camManager* _camMgr);
	~tileManager();
	mapTile& getTile(int index);
	void updateTileList(sCtrlParam& param);
	void uploadTile();
	RendererEle& getRenderEle(short idx);

	std::vector<int> tileList;        // tile list to render

	unsigned int tbo;
	unsigned int ibo;

private:
	LRUCache *_lru;
	dataCache *_dataMgr;
	renderCache *_renderMgr;
	camManager* _camMgr;
	mapTile _tileCache[TILE_CACHE_SIZE+ ROOT_TILE_CACHE_SIZE];
	mapTile* _root[8];
	std::queue<int> _uploadList;       // tile queue to upload data to GPU

	bool _checkTileVisible(mapTile& tile);
	void _init();
	int _getFreeTile(tileId& id);
	int _getTileIndex(tileId& id);
	void _updateTileIndex(tileId& id);
	unsigned long _backgroundProcess();
	bool _updateRenderElement(mapTile& tile);

	int _getDataIndex(mapTile& tile);	
    int _getRenderIndex(mapTile& tile);

};
