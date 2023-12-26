#include "map_tile.h"
#include <math.h>

tileId::tileId() {
	this->level = -1;
	this->xidx = -1;
	this->yidx = -1;
}

tileId::tileId(short l, short x, short y) {

	this->level = l;
	this->xidx = x;
	this->yidx = y;
}
int tileId::getKey() {
	int _l = (level & 0x000F) << 24;
	int _x = (xidx & 0x3FFF) << 14;
	int _y = (yidx & 0x3FFF) ;
	return  _l | _x | _y;
}

tileId tileId::getChild(short index) {
	tileId ret;
	if (index < 0 || index >= 4) {
		return ret;
	}
	ret.level = level + 1;
	ret.xidx = xidx * 2 + index % 2;
	ret.yidx = yidx * 2 + index / 2;
	return ret;
}
mapTile::mapTile(tileId id) {
	this->id = id;
	this->dataIdx = -1;
	this->renderIdx = -1;
	this->child[0] = 0;
	this->child[1] = 0;
	this->child[2] = 0;
	this->child[3] = 0;

	float step = 90.0 / powf(2.0, id.level);
	this->bbx.l = -180.0 + step * id.xidx;
	this->bbx.b = -90+step * id.yidx;
	this->bbx.r = bbx.l + step;;
	this->bbx.t = bbx.b + step;
}

void mapTile::updateBBX() {
	if (this->id.level == -1) {
		return;
	}

	float step = 90.0 / powf(2.0, id.level);
	this->bbx.l = -180.0 + step * id.xidx;
	this->bbx.b = -90 + step * id.yidx;
	this->bbx.r = bbx.l + step;;
	this->bbx.t = bbx.b + step;
}

mapTile::mapTile()
{
	this->id = tileId();
	this->dataIdx = -1;
	this->renderIdx = -1;
	this->child[0] = 0;
	this->child[1] = 0;
	this->child[2] = 0;
	this->child[3] = 0;

	this->bbx.l = -360;
	this->bbx.b = -360;
	this->bbx.r = -360;
	this->bbx.t = -360;
}

mapTile::~mapTile()
{
	for (int i = 0; i < 4; i++) {
		if (child[i])
			delete child[i];
	}
}