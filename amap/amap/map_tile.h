#pragma once

class tileId
{
public:
	unsigned short level;
	unsigned short xidx;
	unsigned short yidx;
	tileId();
	tileId(unsigned short l, unsigned short x, unsigned short y);
};

class mapTile
{
public:
	tileId id;

	// cache data
	short dataIdx;

	// render obj
	short renderIdx;

	mapTile();
	~mapTile();

private:

};