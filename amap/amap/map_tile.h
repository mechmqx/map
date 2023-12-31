#pragma once
#include "type_define.h"
#include <string>

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
	void updateBBX();

private:

};
