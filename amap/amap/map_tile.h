#pragma once

class tileId
{
public:
	unsigned short level;
	unsigned short xidx;
	unsigned short yidx;
	int getKey();
	tileId();
	tileId(unsigned short l, unsigned short x, unsigned short y);
};

class mapTile
{
public:
	tileId id;

	// cache data
	int dataIdx;

	// render obj
	int renderIdx;

	mapTile();
	~mapTile();

private:

};