#include "tile_mgr.h"
#include "esUtil.h"
#include "image_load.h"

#include <thread>
#include <assert.h>

mapTile& tileManager::getTile(int index) {
	return this->tileCache[index];
}
int tileManager::getFreeTile(int key) {
	eIRUState state = eIRUNew;
	return this->_lru->get(key, state);
}

// todo: not use this api, use input level; and use frustum clip to judge tile visiblity;
// use background thread to load data, traverse the data pool;
int tileManager::addTile(tileId& id) {
	// 1. get a free tile
	int idx = getFreeTile(id.getKey());

	// 2. fill tile
	auto& tile = tileCache[idx];
	tile.id = id;
	tile.dataIdx = this->dataMgr->getFreeCacheIndex(id.getKey());
	tile.renderIdx = this->renderMgr->getFreeCacheIndex(id.getKey());

	// 3.add tile index into list
	this->tileList.push_back(idx);

	return 0;
}

// traverse the data pool and load the data from file system
unsigned long tileManager::backgroundProcess() {
	for (;;) {

		for (int i = 0; i < TILE_CACHE_SIZE; i++)
		{
			auto& tile = this->tileCache[i];

			if (tile.dataIdx == -1) { continue; }

			auto& cache = this->dataMgr->cache[tile.dataIdx];

			if (cache.state == eWaitLoading) {
				cache.state = eLoadVertex;
				// 1. gen vertex
				cache.genVertex(tile.id);

				cache.state = eLoadImage;
				// 2. load image
				cache.loadTexture(tile.id);

				cache.state = eReady;
			}
		}
	}
	return 0;
}

void tileManager::UpdateRenderEle(mapTile& tile) {
	auto& cache = this->dataMgr->cache[tile.dataIdx];
	if (cache.state != eReady)
		return;

	this->renderMgr->updateEle(tile.renderIdx, cache);
}
RendererEle& tileManager::getRenderEle(short idx) {
	auto& ele = renderMgr->getElement(idx);

	return ele;
}

tileManager::tileManager()
{
	this->dataMgr = new dataCache();
	this->renderMgr = new renderCache();
	this->_lru = new LRUCache(TILE_CACHE_SIZE);

	// task spawn: data loading thread
	std::thread t(&tileManager::backgroundProcess, this);
	t.detach();
	
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
}
