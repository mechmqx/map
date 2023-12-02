#include "tile_mgr.h"

mapTile* tileManager::getTile(short index) {
	return &this->tileCache[index];
}
int tileManager::getFreeTile() {
	return 0;
}

tileManager::tileManager()
{
	this->dataMgr = new dataCache();
	this->renderMgr = new renderCache();
	
}

tileManager::~tileManager()
{
	delete this->dataMgr;
	delete this->renderMgr;

}
