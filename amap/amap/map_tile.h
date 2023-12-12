#pragma once
#include "type_define.h"

class tileId
{
public:
	unsigned short level;
	unsigned short xidx;
	unsigned short yidx;
	int getKey();
	tileId();
	tileId(unsigned short l, unsigned short x, unsigned short y);
	tileId getChild(short index);
};


class mapTile
{
public:
	tileId id;
	sAABB bbx;
	mapTile* child[4];

	// cache data
	int dataIdx;

	// render obj
	int renderIdx;

	mapTile();
	mapTile(tileId id);
	~mapTile();

private:

};
