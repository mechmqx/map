#include "tile_mgr.h"
#if USE_OPENGL_MODEN
#include "glad.h"
#else
#include "esUtil.h"
#endif
#include "image_load.h"

#include <thread>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <unordered_map>

mapTile& tileManager::getTile(int index) {
	return this->tileCache[index];
}

int tileManager::_getTileIndex(tileId& id) {
	return _lru->getindex(id);
}

void tileManager::_updateTileIndex(tileId& id) {
	_lru->update(id);
	this->dataMgr->updateCacheIndex(id);
	this->renderMgr->updateCacheIndex(id);
}

int tileManager::_getFreeTile(tileId& id) {
	sIRUState state = { eIRUFresh,tileId() };
	int idx = -1;
	do {

	    idx = this->_lru->get(id, state);
	} while (tileCache[idx].getState() == eTileDataLoading);

	if (idx < 0 || idx >= TILE_CACHE_SIZE) {
		int a = 0;
	}
	assert(idx >= 0);
	assert(idx < TILE_CACHE_SIZE);

	tileCache[idx].setId(id);

	return idx;
}

int tileManager::_getDataIndex(mapTile& tile) {
	tileId oldid;
	int ret = this->dataMgr->getFreeCacheIndex(tile.getId(), oldid);
	if (oldid.isValid()&&oldid!=tile.getId()) {
		int idx = this->_lru->getindex(oldid);

		if (idx >= 0 && idx < TILE_CACHE_SIZE) {		
			tileCache[idx].dataIdx = -1;

			std::cout << "Data cache[" << ret << "] " << tileCache[idx].getId().getStr() << "->" << tile.getId().getStr() << std::endl;
			std::cout << "Reset Tile[" << idx << "] 's dataIdx" << std::endl;	
		}	
	}
	else if(oldid != tile.getId()){

		std::cout << "Data cache[" << ret << "] ------ ->" << tile.getId().getStr() << std::endl;
	}
	else {
		// fix tile data cached case
		if (tile.getState() < eTileDataReady) {
			tile.setState(eTileDataReady);
		}
	}

	if (ret == DATA_CACHE_SIZE-1) {
		int a = 0;
	}
	return ret;
}

int tileManager::_getRenderIndex(mapTile& tile) {
	tileId oldid;
	int ret = this->renderMgr->getFreeCacheIndex(tile.getId(), oldid);
	if (oldid.isValid()&& oldid != tile.getId()) {
		int idx = this->_lru->getindex(oldid);
		if (idx >= 0 && idx < TILE_CACHE_SIZE) {
			tileCache[idx].renderIdx = -1;
			std::cout << "Render cache["<< ret<<"] "<< tileCache[idx].getId().getStr()<<"->"<< tile.getId().getStr() << std::endl;
			std::cout << "Reset Tile[" << idx << "] 's render Idx" << std::endl;

			// fix the status error
			if (tileCache[idx].dataIdx != -1) {
				tileCache[idx].setState(eTileDataReady);
			}
			else {
				tileCache[idx].setState(eTileNew);
			}		

		}
		else {
			// fix error idx(-1), if tile with data but not in tile-cache, do nothing. do not set cache index with error idx, mv setting into pre if-case.
		}

	}
	else if(oldid != tile.getId()){
		std::cout << "Render cache[ " << ret << "] ------ ->" << tile.getId().getStr()  << std::endl;
	}
	if (ret == RENDER_CACHE_SIZE-1) {
		int a = 0;
	}
	return ret;
}

void tileManager::uploadTile() {

	//find upload tile
	for (int i = 0; i < TILE_CACHE_SIZE + ROOT_TILE_CACHE_SIZE; i++)
	{
		auto& tile = this->tileCache[i];

		if (tile.dataIdx < 0 || tile.dataIdx >= DATA_CACHE_SIZE + ROOT_TILE_CACHE_SIZE) { continue; }

		if (tile.getState() == eTileDataReady) {
			uploadList.push(i);
		}
	}
#define UPLOAD_NUM_PER_FRAME (2)
	int count = 0;
	while (!uploadList.empty())
	{
		int idx = uploadList.front();
		mapTile& tile = tileCache[idx];
		if (tile.getState() == eTileDrawable) {
			uploadList.pop();
			continue;
		}
		if (tile.getState() != eTileDataReady) {
			uploadList.pop();    // rm status error tiles
			continue;    // todo, now:if tile data not ready, check next frame
		}

		_updateRenderElement(tile);
		std::cout << "====== Upload Tile(" << tile.getId().getStr() << ") to GPU ======" << std::endl;
		if (tile.getId().xidx == 54 && tile.getId().yidx == 20 && tile.getId().level == 4) {
			std::cout << std::endl;
		}

		tile.setState(eTileDrawable);

		uploadList.pop();
		count++;
		if (count >= UPLOAD_NUM_PER_FRAME) {
			break;
		}
	}
#undef UPLOAD_NUM_PER_FRAME
}

bool tileManager::_checkTileVisible(mapTile& tile) {
	Vec3d center = { (tile.bbx.l + tile.bbx.r) / 2,(tile.bbx.t + tile.bbx.b) / 2,0.0 };
	double radius = (tile.bbx.r - tile.bbx.l) * 1.415;
	return camMgr->pointInFrumstum(&center, radius);
}

#define MAX_LEVEL (8)
void tileManager::updateTileList(sCtrlParam& param) {
	// 0. clear tileList
	tileList.clear();

	// 1. calculate current level
	int level = floor(log2f(140.0 / param.range));
	if (level > MAX_LEVEL) {
		level = MAX_LEVEL;
	}

	std::queue<mapTile*> stack;
	// 2.traverse the root tiles
	for (int i = 0; i < 8; i++) {
		mapTile* pRoot = root[i];
		stack.push(pRoot);
	}

	// check node
	while(!stack.empty()) {
		mapTile* pTile = stack.front();
		stack.pop();

		// 3. check visibility
		if (!_checkTileVisible(*pTile))
			continue;

		// 3.1 make child visible flag
		pTile->childVisible = 0;
		for (int i = 0; i < 4; i++) {
			// 3.1.1 create child-for-check
			tileId&& childid = pTile->getId().getChild(i);
			auto child = new mapTile(childid);
			child->updateBBX();

			// 3.1.2 check visible
			bool childVisible = _checkTileVisible(*child);

			// 3.1.3 if child not empty, check validation
			if (pTile->child[i] && pTile->child[i]->getId() != child->getId())
			{
				pTile->child[i] = 0;
			}

			delete child;

			// 3.1.4 make the flag
			pTile->childVisible |= ((childVisible ? 0b1 : 0x0) << i);

		}

		// 4. check max level
		bool childReady = true;
		if (pTile->getId().level < level) {
			for (int i = 0; i < 4; i++) {
				bool childVisible = pTile->childVisible & (0b1<<i);

				// 4.1 child empty, not ready
				if (childVisible && pTile->child[i] == 0) {
					childReady = false;
					break;
				}

				// 4.2 visible & not ready -> not ready
				if (childVisible && (pTile->child[i]->getState() != eTileDrawable)) {
					childReady = false;
					break;
				}
			}
		}
		else { 
			childReady = false; 
		}

		// 5. child ready?
		if (childReady&& pTile->childVisible!=0)   // at least a child is visible, and all children visible are ready
		{
			for (int i = 0; i < 4; i++) {
				bool childVisible = (pTile->childVisible & 0b1 << i);
				if (childVisible && pTile->child[i]) {
					_updateTileIndex(pTile->child[i]->getId());
				    stack.push(pTile->child[i]);
				}
			}

			continue; // process next node
		}
		else// child not ready, check current node
		{
			if (pTile->getState() == eTileDrawable) {
				if (pTile->getId().level == 0) {
					int idx = TILE_CACHE_SIZE + pTile->getId().xidx + 4 * pTile->getId().yidx;
					tileList.push_back(idx);
				}
				else {
				    tileList.push_back(_lru->getindex(pTile->getId()));
					_updateTileIndex(pTile->getId());
				}
			}
			else {
				//if(pTile->childVisible != 0) //all child not visible or not checked yet, to check childs
				    continue; // if current Tile is not ready, do not traverse child
			}
		}

		// 6. check childs
		if (pTile->getId().level < level) {
			for (int i = 0; i < 4; i++) {
				if (pTile->childVisible & 0b1 << i)    // if visible
				{
					// 6.1 child empty, get child
					if (pTile->child[i] == 0)    
					{
						tileId&& childid = pTile->getId().getChild(i);
						int freeidx = _getFreeTile(childid);
						pTile->child[i] = &tileCache[freeidx];
						pTile->child[i]->father_idx = pTile->cur_idx;
					}				

					// 6.2 check status
					pTile->child[i]->dataIdx = _getDataIndex(*pTile->child[i]);
					pTile->child[i]->renderIdx = _getRenderIndex(*pTile->child[i]);
				}
			}
		}
		
	}
}

unsigned long tileManager::_backgroundProcess() {
	for (;;) {

		for (int i = 0; i < TILE_CACHE_SIZE; i++)
		{
			auto& tile = this->tileCache[i];

			if (tile.dataIdx <0 || tile.dataIdx>= DATA_CACHE_SIZE) { continue; }

			auto& cache = this->dataMgr->cache[tile.dataIdx];
			cache.lockCache();
			if (cache.state == eWaitLoading) {
				tile.setState(eTileDataLoading);
				cache.state = eLoadVertex;
				// 1. gen vertex
				cache.genVertex(tile.getId());

				cache.state = eLoadImage;
				// 2. load image
				cache.loadTexture(tile.getId());

				cache.state = eReady;

				tile.setState(eTileDataReady);
				//std::cout << "----- Load Tile["<<i <<"](" << tile.id.getStr() << ") data to Cache("<< tile.dataIdx <<") -----" << std::endl;
				
			}
			cache.unlockCache();
		}
	}
}

void tileManager::_updateRenderElement(mapTile& tile) {
	auto& cache = this->dataMgr->cache[tile.dataIdx];
	cache.lockCache();
	if (cache.state != eReady){
		cache.unlockCache();
		return;
	}

	this->renderMgr->updateEle(tile.renderIdx, cache);
	cache.unlockCache();
}
RendererEle& tileManager::getRenderElement(short idx) {
	auto& ele = renderMgr->getElement(idx);

	return ele;
}

tileManager::tileManager(camManager* camMgr)
{
	this->camMgr = camMgr;
	this->_init();
}
void tileManager::_init()
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

	// init tile pool
	for (int i = 0; i < TILE_CACHE_SIZE+ ROOT_TILE_CACHE_SIZE; i++) {
		auto& tile = tileCache[i];
		tile.setId( tileId() );
		tile.dataIdx = -1;
		tile.renderIdx = -1;
		tile.setState( eTileStateNum);
		tile.childVisible = 0;
		tile.bbx.l = 360;
		tile.bbx.r = -360;
		tile.bbx.b = 180;
		tile.bbx.t = -180;
		tile.child[0] = NULL;
		tile.child[1] = NULL;
		tile.child[2] = NULL;
		tile.child[3] = NULL;
		tile.father_idx = -1;
		tile.cur_idx = i;
	}

	// root tile
	for (int i = 0; i < 8; i++) {
		tileId id = tileId(0, i % 4, i / 4);
		root[i] = &tileCache[TILE_CACHE_SIZE + i];
		root[i]->setId(id);
		root[i]->father_idx = -1;
				
		// 1. get data tem use
		cacheEle* data = new cacheEle();
		data->genVertex(id);
		data->loadTexture(id);

		// 2. get render cache
		root[i]->renderIdx = RENDER_CACHE_SIZE + i;
		RendererEle& ele = renderMgr->getElement(root[i]->renderIdx);

		ele.state = eRenderLoading;
		glBindBuffer(GL_ARRAY_BUFFER, ele.vbo);
		int debug = sizeof(Vec2f);
		glBufferSubData(GL_ARRAY_BUFFER, 0, TILE_V_NUM * sizeof(Vec2f), data->vert);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, ele.texId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)IMG_SIZE,
			(GLsizei)IMG_SIZE, GL_RGB,
			GL_UNSIGNED_BYTE, data->image);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFlush();
		ele.state = eRenderReady;
		root[i]->setState(eTileDrawable);
		delete data;
	}

	// task spawn: data loading thread
	std::thread t(&tileManager::_backgroundProcess, this);
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

	unsigned short index[TILE_I_NUM];
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TILE_I_NUM*sizeof(unsigned short), index, GL_STATIC_DRAW);

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
