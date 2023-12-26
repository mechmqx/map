#pragma once
#include <vector>
#include "data_cache.h"
#include "renderer_cache.h"
#include "map_tile.h"
#include "type_define.h"
#include "lru_cache.h"
#include "cam_mgr.h"
#include <unordered_map>

#define TILE_CACHE_SIZE (64)

class tileManager
{
public:
	tileManager();
	tileManager(camManager* camMgr);
	~tileManager();
	mapTile& getTile(int index);
	int addTile(tileId& id);
	void updateTileList(sCtrlParam param);

	std::vector<int> tileList;

	RendererEle& getRenderEle(short idx);
	void UpdateRenderEle(mapTile& tile);
	unsigned int tbo;
	unsigned int ibo;
	unsigned int gTBO;
	unsigned int gIBO;
	unsigned int gVBO;
	unsigned int gTexId;
	unsigned long foregroundProcess();

private:
	LRUCache *_lru;
	dataCache *dataMgr;
	renderCache *renderMgr;
	camManager* camMgr;
	int getFreeTile(int key);
	mapTile tileCache[TILE_CACHE_SIZE];
	unsigned long backgroundProcess();
	void checkTileTree(int level, mapTile* tile);
	std::unordered_map<std::string, int> _list_umap;

	mapTile* root[8];
};
