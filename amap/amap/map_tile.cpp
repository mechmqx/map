#include "map_tile.h"


tileId::tileId() {
	this->level = -1;
	this->xidx = -1;
	this->yidx = -1;
}

tileId::tileId(unsigned short l, unsigned short x, unsigned short y) {

	this->level = l;
	this->xidx = x;
	this->yidx = y;
}

mapTile::mapTile()
{
	this->id = tileId();
	this->dataIdx = -1;
	this->renderIdx = -1;
}

mapTile::~mapTile()
{
}