#pragma once
#include "tile_id.h"
#include <string>
#include <Windows.h>

typedef enum tagTileState {
	eTileNew,
	eTileDataReady,
	eTileDrawable,
	eTileStateNum
}eTileState;

typedef enum tagChildVisble {
	eInvisble = 0b00000000,
	eChild00Visble = 0b00000001,
	eChild01Visble = 0b00000010,
	eChild10Visble = 0b00000100,
	eChild11Visble = 0b00001000,
}eChildVisble;

class mapTile
{
	HANDLE mutex;
public:
	tileId id;
	sAABB bbx;
	mapTile* child[4];

	char childVisible;

	eTileState tilestate;

	// cache data
	int dataIdx;

	// render obj
	int renderIdx;

	mapTile();
	mapTile(tileId& id);
	~mapTile();
	void updateBBX();
	int setId(tileId& id);

	void setState(eTileState state);

private:

};
