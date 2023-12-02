#pragma once

class tileId
{
public:
	unsigned short level;
	unsigned short xidx;
	unsigned short yidx;
	tileId();
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