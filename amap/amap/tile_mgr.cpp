#include "tile_mgr.h"
#include "esUtil.h"
#include "image_load.h"

#include <thread>
#include <assert.h>
#include <math.h>
#include <iostream>

mapTile& tileManager::getTile(int index) {
	return this->tileCache[index];
}
int tileManager::getTileIndex(int key) {
	return _lru->getindex(key);
}
int tileManager::getFreeTile(int key) {
	sIRUState state = { eIRUFresh,-1 };
	int idx = this->_lru->get(key, state);
	if (state.state == eIRUReuse) {
		// if reuse node, clear old node
		auto& iter = _list_umap.find(state.oldKey);
		if (iter != _list_umap.end())
		{
			int idx = iter->second;
			tileCache[idx].renderIdx = -1;
			tileCache[idx].dataIdx = -1;
			tileCache[idx].child[0] = NULL;
			tileCache[idx].child[1] = NULL;
			tileCache[idx].child[2] = NULL;
			tileCache[idx].child[3] = NULL;

			_list_umap.erase(state.oldKey);
		}
	}
	return idx;
}

int tileManager::getDataIndex(mapTile tile) {
	int oldkey = -1;
	int ret = this->dataMgr->getFreeCacheIndex(tile.id.getKey(), oldkey);
	if (oldkey != -1) {
		int idx = this->_lru->getindex(oldkey);
		if (idx > 0 && idx < TILE_CACHE_SIZE) {
			tileCache[idx].dataIdx = -1;
		}
	}
	return ret;
}
int tileManager::getRenderIndex(mapTile tile) {
	int oldkey = -1;
	int ret = this->dataMgr->getFreeCacheIndex(tile.id.getKey(), oldkey);
	if (oldkey != -1) {
		int idx = this->_lru->getindex(oldkey);
		if (idx > 0 && idx < TILE_CACHE_SIZE) {
			tileCache[idx].renderIdx = -1;
		}
	}
	return ret;
}

// remove the subtree
int tileManager::rmTile(mapTile& tile) {
	// remove
	return 0;
}
int tileManager::addTile(mapTile& tile) {
	// 0. check umap
	auto key = tile.id.getKey();
	if (_list_umap.find(key) != _list_umap.end()) {
		return 0;
	}

	// 1. get tile idx and push into list
	int idx = getTileIndex(tile.id.getKey());
	if (idx == -1) {
		return -1;
	}
	tileList.push_back(idx);
	_list_umap.insert(std::pair<int,int>(tile.id.getKey(), idx));

	// 2. fill tile attribs
	tile.dataIdx = getDataIndex(tile);
	tile.renderIdx = getRenderIndex(tile);
	std::cout << "add(" << tile.id.getStr() << ") to list,tileIdx="<<idx<<" ,dataIdx = " << tile.dataIdx << ", renderIdx = " << tile.renderIdx << std::endl;

	return 0;
}
static int depth = 0;

void tileManager::checkTileTree(int level, mapTile* tile) {
	if (depth > 12) {
		printf("");
	}
	// 1. check level
	if (tile->id.level > level)
		return;
	
	// 2.check visible
	Vec3d center = { (tile->bbx.l + tile->bbx.r) / 2,(tile->bbx.t + tile->bbx.b) / 2,0.0 };
	double radius = (tile->bbx.r - tile->bbx.l)*1.415;
	if (!camMgr->pointInFrumstum(&center, radius))
	{
		// remove subtree otherwise level 0
		if (tile->id.level != 0) {
		    rmTile(*tile);
		}
		// return
		return;

	}

	depth++;

	// 3. add tile to update list
	addTile(*tile);

	// 4. recruse child tile
	for (int i = 0; i < 4; i++) {
		if (!tile->child[i]) {			
			tileId id = tile->id.getChild(i);

			// 1. get a free tile
			int idx = getFreeTile(id.getKey());

			// 2. fill tile
			tile->child[i] = &tileCache[idx];

			tile->child[i]->id = id;
			tile->child[i]->updateBBX();
		}
		assert(tile->id.level + 1 == tile->child[i]->id.level);

		checkTileTree(level, tile->child[i]);
	}
	depth--;
}

void tileManager::updateTileList(sCtrlParam param) {
	// 1. calculate current level
	int level = floor(log2f(140.0 / param.range));
	if (level > 16) {
		level = 16;
	}

	// 2. try add tile to list
	for (int i = 0; i < 8; i++) {
		mapTile* pRoot = root[i];
		
		checkTileTree(level, pRoot);
	}
}

// traverse the data pool and load the data from file system
unsigned long tileManager::foregroundProcess() {
	static int index = 0;
	int checkcnt = 0;

	for (int i = 0; i < 4;)
	{
		int idx = index + checkcnt;
		idx %= TILE_CACHE_SIZE;
		auto& tile = this->tileCache[idx];

		checkcnt++;
		if (tile.dataIdx < 0 || tile.dataIdx >= DATA_CACHE_SIZE) { continue; }

		// when update a real data, update the i value
		i++;

		auto& cache = this->dataMgr->cache[tile.dataIdx];
		cache.lockCache();
		if (cache.state == eWaitLoading) {
			cache.state = eLoadVertex;
			// 1. gen vertex
			cache.genVertex(tile.id);

			cache.state = eLoadImage;
			// 2. load image
			cache.loadTexture(tile.id);

			cache.state = eReady;
		}
		cache.unlockCache();
	}
	index += checkcnt;
	index /= TILE_CACHE_SIZE;
	
	return 0;
}
unsigned long tileManager::backgroundProcess() {
	for (;;) {

		for (int i = 0; i < TILE_CACHE_SIZE; i++)
		{
			auto& tile = this->tileCache[i];

			if (tile.dataIdx <0 || tile.dataIdx>= DATA_CACHE_SIZE) { continue; }

			auto& cache = this->dataMgr->cache[tile.dataIdx];
			cache.lockCache();
			if (cache.state == eWaitLoading) {
				cache.state = eLoadVertex;
				// 1. gen vertex
				cache.genVertex(tile.id);

				cache.state = eLoadImage;
				// 2. load image
				cache.loadTexture(tile.id);

				cache.state = eReady;
			}
			cache.unlockCache();
		}
	}
}

void tileManager::UpdateRenderEle(mapTile& tile) {
	auto& cache = this->dataMgr->cache[tile.dataIdx];
	cache.lockCache();
	if (cache.state != eReady){
		return;
	}

	this->renderMgr->updateEle(tile.renderIdx, cache);
	cache.unlockCache();
}
RendererEle& tileManager::getRenderEle(short idx) {
	auto& ele = renderMgr->getElement(idx);

	return ele;
}

tileManager::tileManager(camManager* camMgr)
{
	this->camMgr = camMgr;
	this->tileManager::tileManager();
}
tileManager::tileManager()
{
	this->dataMgr = new dataCache();
	this->renderMgr = new renderCache();
	this->_lru = new LRUCache(TILE_CACHE_SIZE);
	if (!this->camMgr) {
		sCtrlParam ctrl;
		ctrl.lon = 120.0;
		ctrl.lat = 30.0;
		ctrl.range = 400 / 108.0;

		int viewport[4] = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		this->camMgr = new camManager(ctrl, viewport);
	}

	// root tile
	for (int i = 0; i < 8; i++) {
		tileId id = tileId(0, i % 4, i / 4);
		root[i] = new mapTile(id);
		
		// 1. get data tem use
		cacheEle* data = new cacheEle();
		data->genVertex(id);
		data->loadTexture(id);

		// 2. get render cache
		root[i]->renderIdx = RENDER_CACHE_SIZE + 1;
		RendererEle& ele = renderMgr->getElement(root[i]->renderIdx);

		ele.state = eRenderLoading;
		glBindBuffer(GL_ARRAY_BUFFER, ele.vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, TILE_V_NUM * 2 * 4, data->vert);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, ele.texId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)IMG_SIZE,
			(GLsizei)IMG_SIZE, GL_RGB,
			GL_UNSIGNED_BYTE, data->image);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFlush();
		ele.state = eRenderReady;
		delete data;
	}

#if USE_BACKGROUND_TASK
	// task spawn: data loading thread
	std::thread t(&tileManager::backgroundProcess, this);
	t.detach();
#endif	
	/*
	unsigned int tbo;
	unsigned int ibo;*/
	glGenBuffers(1, &tbo);
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &gTBO);
	glGenBuffers(1, &gIBO);
	glGenBuffers(1, &gVBO);
	glGenTextures(1, &gTexId);

	short index[TILE_I_NUM];
	Vec2f coords[TILE_V_NUM];
	// gen Coords
	for (int j = 0; j < GRID_SIZE; j++) {
		for (int i = 0; i < GRID_SIZE; i++) {
			int idx = j*GRID_SIZE + i;
			coords[idx].x = (float)i / (GRID_SIZE-1);
			coords[idx].y = 1.0-(float)j / (GRID_SIZE - 1);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, TILE_V_NUM*sizeof(Vec2f), coords, GL_STATIC_DRAW);

	// genIndex
	for (int j = 0; j < GRID_SIZE - 1; j++) {
		static int idx = 0;
		for (int i = 0; i < GRID_SIZE; i++) {
			index[idx] = i + j * GRID_SIZE;
			idx++;
			index[idx] = i + (j + 1) * GRID_SIZE;
			idx++;
		}
		if (j < GRID_SIZE - 2) {
			index[idx] = (j + 1) * GRID_SIZE + GRID_SIZE - 1;
			idx++;
			index[idx] = (j + 1) * GRID_SIZE;
			idx++;
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	Vec2f gVertex[4] = { {-180,-90},{180,-90},{-180,90},{180,90} };
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vec2f), gVertex, GL_STATIC_DRAW);
	Vec2f gCoord[4] = { {0.0,1.0},{1.0,1.0},{0.0,0.0},{1.0,0.0} };
	glBindBuffer(GL_ARRAY_BUFFER, gTBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vec2f), gCoord, GL_STATIC_DRAW);
	unsigned short gIndex[4] = { 0,2,1,3 };
	glBindBuffer(GL_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(unsigned short), gIndex, GL_STATIC_DRAW);
	
	std::string filename = "views.png";
	int w, h, c;
	unsigned char* pixels;
	if (map_load_image(filename, w, h, c, &pixels)) {
		assert(c == 4);
		glBindTexture(GL_TEXTURE_2D, gTexId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			(GLsizei)w,
			(GLsizei)h,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			(GLvoid*)pixels);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	map_destroy_image(&pixels);

}

tileManager::~tileManager()
{
	delete this->dataMgr;
	delete this->renderMgr;

	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &tbo);

	delete _lru;

	for (int i = 0; i < 8; i++) {
		delete root[i];
	}
}
