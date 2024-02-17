#pragma once
#include "type_define.h"
#include <string>

#define INVALID_KEY (-1)
#define MAX_LEVEL_BIT (4)
#define MAX_INDEX_BIT (14)
#define MAX_LEVEL (0x1<<MAX_LEVEL_BIT-1)
#define MAX_INDEX (0x1<<MAX_INDEX_BIT-1)

class tileId
{
	bool bValid;
	void setId(short l, short x, short y);
public:
	short level;
	short xidx;
	short yidx;
	int getKey();
	std::string getStr();
	tileId();
	tileId(short l, short x, short y);
	tileId getChild(short index);
	bool isValid();
	bool checkValid();

	bool operator==(const tileId& other)
	{
		if (level != other.level || xidx != other.xidx || yidx != other.yidx) {
			return false;
		}
		return true;
	}
	bool operator!=(const tileId& other)
	{
		if (level != other.level || xidx != other.xidx || yidx != other.yidx) {
			return true;
		}
		return false;
	}
};
