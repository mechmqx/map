#include "tile_mgr.h"
#include "esUtil.h"

mapTile* tileManager::getTile(short index) {
	return &this->tileCache[index];
}
int tileManager::getFreeTile() {
	return 0;
}

// todo: not use this api, use input level; and use frustum clip to judge tile visiblity;
// use background thread to load data, traverse the data pool;
int tileManager::addTile(tileId& id) {
	// 1. get a free tile

	// 2. add tile index into list

	// 3. 

	return 0;
}

RendererEle& tileManager::getRederEle(short idx) {
	return renderMgr->getElement(idx);
}

tileManager::tileManager()
{
	this->dataMgr = new dataCache();
	this->renderMgr = new renderCache();
	
	/*
	unsigned int tbo;
	unsigned int ibo;*/
	glGenBuffers(1, &tbo);
	glGenBuffers(1, &ibo);

	short index[TILE_I_NUM];
	Vec2f coords[TILE_V_NUM];
	// gen Coords
	for (int j = 0; j < GRID_SIZE; j++) {
		for (int i = 0; i < GRID_SIZE; i++) {
			int idx = j*GRID_SIZE + i;
			coords[idx].x = (float)i / (GRID_SIZE-1);
			coords[idx].y = (float)j / (GRID_SIZE - 1);
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
}

tileManager::~tileManager()
{
	delete this->dataMgr;
	delete this->renderMgr;

	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &tbo);
}
