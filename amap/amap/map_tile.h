#pragma once
#include "type_define.h"
#include <string>

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

class tileId
{
public:
	short level;
	short xidx;
	short yidx;
	int getKey();
	std::string getStr();
	tileId();
	tileId(short l, short x, short y);
	tileId getChild(short index);

	bool operator==(const tileId& other)
	{
		if (level != other.level || xidx != other.xidx && yidx != other.yidx) {
			return false;
		}
		return true;
	}
	bool operator!=(const tileId& other)
	{
		if (level != other.level || xidx != other.xidx && yidx != other.yidx) {
			return true;
		}
		return false;
	}
};


class mapTile
{
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

private:

};
