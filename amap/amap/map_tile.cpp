#include "map_tile.h"
#include <math.h>
#include <assert.h>

#include <iostream>

mapTile::mapTile(tileId& id) {
	setId(id);
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void mapTile::setState(eTileState state) {
	WaitForSingleObject(mutex, INFINITE);

	this->tilestate = state;

	std::cout << "tile(" << id.getStr() << ") set ";
	switch (state)
	{
	case eTileNew:
		std::cout << "eTileNew";
		break;
	case eTileDataReady:
		std::cout << "eTileDataReady";
		break;
	case eTileDrawable:
		std::cout << "eTileDrawable";
		break;
	case eTileStateNum:
		std::cout << "eTileStateNum";
		break;
	default:
		std::cout << "INVALID STATE";
		break;
	}
	std::cout<< std::endl;

	ReleaseMutex(mutex);
}

int mapTile::setId(tileId& id) {
	this->id = id;
	this->updateBBX();
	this->dataIdx = -1;
	this->renderIdx = -1;
	this->child[0] = 0;
	this->child[1] = 0;
	this->child[2] = 0;
	this->child[3] = 0;

	float step = 90.0 / powf(2.0, id.level);
	this->bbx.l = -180.0 + step * id.xidx;
	this->bbx.b = -90 + step * id.yidx;
	this->bbx.r = bbx.l + step;;
	this->bbx.t = bbx.b + step;

	this->childVisible = 0;
	this->tilestate = eTileNew;
	return 0;
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

	this->childVisible = 0;
	this->tilestate = eTileNew;
	mutex = CreateMutex(NULL, FALSE, NULL);
}

mapTile::~mapTile()
{
	for (int i = 0; i < 4; i++) {
		if (child[i])
			delete child[i];
	}
	CloseHandle(mutex);
}