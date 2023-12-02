#include "map_tile.h"


tileId::tileId() {
	this->level = -1;
	this->xidx = -1;
	this->yidx = -1;
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