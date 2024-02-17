#include "tile_id.h"
#include <math.h>
#include <assert.h>

void tileId::setId(short l, short x, short y) {
	this->level = l;
	this->xidx = x;
	this->yidx = y;
	checkValid();
}

tileId::tileId() {
	setId(-1, -1, -1);
}

tileId::tileId(short l, short x, short y) {
	setId(l, x, y);
}

std::string tileId::getStr() {
	if (!bValid)
		return "";
	return std::to_string(level) + "_" + std::to_string(xidx) + "_" + std::to_string(yidx);
}
int tileId::getKey() {
	if (!bValid)
		return INVALID_KEY;

	int _l = level << (MAX_INDEX_BIT+ MAX_INDEX_BIT);
	int _x = xidx << MAX_INDEX_BIT;
	int _y = yidx;
	return  _l | _x | _y;
}

bool tileId::checkValid() {
	if (level < 0) {
		bValid = false;
		return false;
	}
	if (xidx < 0) {
		bValid = false;
		return false;
	}
	if (yidx < 0) {
		bValid = false;
		return false;
	}
	if (xidx >= pow(2,this->level + 2)) {
		bValid = false;
		return false;
	}
	if (yidx >= pow(2, this->level + 1)) {
		bValid = false;
		return false;
	}
	bValid = true;
	return true;
}
bool tileId::isValid() {
	return bValid;
}

tileId tileId::getChild(short index) {
	tileId ret;
	if (!bValid || index < 0 || index >= 4) {
		return ret;
	}
	short _level = level + 1;
	short _xidx = xidx * 2 + index % 2;
	short _yidx = yidx * 2 + index / 2;
	ret.setId(_level, _xidx, _yidx);
	return ret;
}