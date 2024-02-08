#include "tile_mgr.h"
#include "esUtil.h"
#include "image_load.h"

#include <thread>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <unordered_map>

mapTile& tileManager::getTile(int index) {
	return this->tileCache[index];
}

int tileManager::getTileIndex(tileId& id) {
	return _lru->getindex(id.getKey());
}

int tileManager::getFreeTile(tileId& id) {
	int key = id.getKey();
	sIRUState state = { eIRUFresh,-1 };
	int idx = this->_lru->get(key, state);
	assert(idx >= 0);
	assert(idx < TILE_CACHE_SIZE);
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

			tileCache[idx].childVisible = 0;
			tileCache[idx].id = id;
			tileCache[idx].updateBBX();
			tileCache[idx].tilestate = eTileNew;

			_list_umap.erase(state.oldKey);
		}
	}
	else if (state.state == eIRUFresh) {
		tileCache[idx].id = id;
		tileCache[idx].updateBBX();
		tileCache[idx].tilestate = eTileNew;
	}
	else {
		// do nothing for eIRUReady
	}

	_list_umap.insert(std::unordered_map<int,int>::value_type(key,idx));

	return idx;
}

int tileManager::getDataIndex(mapTile& tile) {
	int oldkey = -1;
	int ret = this->dataMgr->getFreeCacheIndex(tile.id.getKey(), oldkey);
	if (oldkey != -1) {
		int idx = this->_lru->getindex(oldkey);
		if (idx > 0 && idx < TILE_CACHE_SIZE) {
			tileCache[idx].dataIdx = -1;

			std::cout << "Data cache: " << ret << " [" << tileCache[idx].id.getStr() << "->" << tile.id.getStr() << "]" << std::endl;
		}
	}
	else {

		std::cout << "Data cache: " << ret << " [ ------ ->" << tile.id.getStr() << "]" << std::endl;
	}
	return ret;
}
int tileManager::getRenderIndex(mapTile& tile) {
	int oldkey = -1;
	int ret = this->renderMgr->getFreeCacheIndex(tile.id.getKey(), oldkey);
	if (oldkey != -1) {
		int idx = this->_lru->getindex(oldkey);
		if (idx > 0 && idx < TILE_CACHE_SIZE) {
			tileCache[idx].renderIdx = -1;
			std::cout << "Render cache: "<< ret<<" ["<< tileCache[idx].id.getStr()<<"->"<< tile.id.getStr()<<"]" << std::endl;
		}
	}
	else {
		std::cout << "Render cache: " << ret << " [ ------ ->" << tile.id.getStr() << "]" << std::endl;
	}
	return ret;
}

void tileManager::uploadTile() {

	//find upload tile
	for (int i = 0; i < TILE_CACHE_SIZE + ROOT_TILE_CACHE_SIZE; i++)
	{
		auto& tile = this->tileCache[i];

		if (tile.dataIdx < 0 || tile.dataIdx >= DATA_CACHE_SIZE + ROOT_TILE_CACHE_SIZE) { continue; }

		if (tile.tilestate == eTileDataReady) {
			uploadList.push(i);
		}
	}
#define UPLOAD_NUM_PER_FRAME (2)
	int count = 0;
	while (!uploadList.empty())
	{
		int idx = uploadList.front();
		mapTile& tile = tileCache[idx];
		if (tile.tilestate == eTileDrawable) {
			uploadList.pop();
			continue;
		}
		if (tile.tilestate != eTileDataReady) {
			continue;    // todo, now:if tile data not ready, check next frame
		}

		UpdateRenderEle(tile);
		std::cout << "====== Upload Tile(" << tile.id.getStr() << ") to GPU ======" << std::endl;
		if (tile.id.xidx == 54 && tile.id.yidx == 20 && tile.id.level == 4) {
			std::cout << std::endl;
		}

		tile.tilestate = eTileDrawable;

		uploadList.pop();
		count++;
		if (count >= UPLOAD_NUM_PER_FRAME) {
			break;
		}
	}
#undef UPLOAD_NUM_PER_FRAME
}

#define MAX_LEVEL (2)
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
		Vec3d center = { (pTile->bbx.l + pTile->bbx.r) / 2,(pTile->bbx.t + pTile->bbx.b) / 2,0.0 };
		double radius = (pTile->bbx.r - pTile->bbx.l) * 1.415;
		if (!camMgr->pointInFrumstum(&center, radius)) {
			continue;
		}

		// 3.1 make child visible flag
		pTile->childVisible = 0;
		for (int i = 0; i < 4; i++) {
			// 3.1.1 create child-for-check
			tileId childid = pTile->id.getChild(i);
			auto child = new mapTile(childid);
			child->updateBBX();

			// 3.1.2 check visible
			Vec3d center = { (child->bbx.l + child->bbx.r) / 2,(child->bbx.t + child->bbx.b) / 2,0.0 };
			double radius = (child->bbx.r - child->bbx.l) * 1.415;
			bool childVisible = camMgr->pointInFrumstum(&center, radius);

			// 3.1.3 if child not empty, check validation
			if (pTile->child[i] && pTile->child[i]->id != child->id) {
				pTile->child[i] = 0;
			}

			delete child;

			// 3.1.4 make the flag
			pTile->childVisible |= ((childVisible ? 0b1 : 0x0) << i);

		}

		// 4. check max level
		bool childReady = true;
		if (pTile->id.level < level) {
			for (int i = 0; i < 4; i++) {
				bool childVisible = pTile->childVisible & (0b1<<i);

				// 4.1 child empty, not ready
				if (childVisible && pTile->child[i] == 0) {
					childReady = false;
					break;
				}

				// 4.2 visible & not ready -> not ready
				if (childVisible && (pTile->child[i]->tilestate != eTileDrawable)) {
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
				if(childVisible&& pTile->child[i])
				    stack.push(pTile->child[i]);
			}

			continue; // process next node
		}
		else// child not ready, check current node
		{
			if (pTile->tilestate == eTileDrawable) {
				if (pTile->id.level == 0) {
					int idx = TILE_CACHE_SIZE + pTile->id.xidx + 4 * pTile->id.yidx;
					tileList.push_back(idx);
				}
				else {
				    tileList.push_back(_lru->getindex(pTile->id.getKey()));
				}
			}
			else {
				//if(pTile->childVisible != 0) //all child not visible or not checked yet, to check childs
				    continue; // if current Tile is not ready, do not traverse child
			}
		}

		// 6. check childs
		if (pTile->id.level < level) {
			for (int i = 0; i < 4; i++) {
				if (pTile->childVisible & 0b1 << i)    // if visible
				{
					// 6.1 child empty, get child
					if (pTile->child[i] == 0) {
						tileId childid = pTile->id.getChild(i);
						int freeidx = getFreeTile(childid);
						pTile->child[i] = &tileCache[freeidx];
					}				

					// 6.2 check status
					if (pTile->child[i]->dataIdx == -1) {
						pTile->child[i]->dataIdx = getDataIndex(*pTile->child[i]);
					}

					if (pTile->child[i]->renderIdx == -1) {
						pTile->child[i]->renderIdx = getRenderIndex(*pTile->child[i]);
					}
				}
			}
		}
		
	}
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

				tile.tilestate = eTileDataReady;
				std::cout << "----- Load Tile(" << tile.id.getStr() << ") data to Cache -----" << std::endl;
				
			}
			cache.unlockCache();
		}
	}
}

void tileManager::UpdateRenderEle(mapTile& tile) {
	auto& cache = this->dataMgr->cache[tile.dataIdx];
	cache.lockCache();
	if (cache.state != eReady){
		cache.unlockCache();
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

	// init tile pool
	for (int i = 0; i < TILE_CACHE_SIZE+ ROOT_TILE_CACHE_SIZE; i++) {
		auto& tile = tileCache[i];
		tile.id = { -1,-1,-1 };
		tile.dataIdx = -1;
		tile.renderIdx = -1;
		tile.tilestate = eTileStateNum;
		tile.childVisible = 0;
		tile.bbx.l = 360;
		tile.bbx.r = -360;
		tile.bbx.b = 180;
		tile.bbx.t = -180;
		tile.child[0] = NULL;
		tile.child[1] = NULL;
		tile.child[2] = NULL;
		tile.child[3] = NULL;
	}

	// root tile
	for (int i = 0; i < 8; i++) {
		tileId id = tileId(0, i % 4, i / 4);
		//int idx = getFreeTile(id);
		root[i] = &tileCache[TILE_CACHE_SIZE + i];
		root[i]->setId(id);
				
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
		root[i]->tilestate = eTileDrawable;
		delete data;
	}

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
