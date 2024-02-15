#pragma once
#include "lru_cache.h"
#include "data_cache.h"
#include "tile_id.h"

#define RENDER_CACHE_SIZE (128)
#define CONST_CACHE_SIZE (8)

enum eRendererState {
	eRenderEmpty,
	eRenderLoading,
	eRenderReady,
};
class RendererEle {
public:
	int vbo;
	int texId;
	eRendererState state;
	tileId debug_id;
};


class renderCache
{
public:
	renderCache();
	~renderCache();

	int getFreeCacheIndex(tileId& id, tileId& oldid);
	void freeCache(short idx);

	RendererEle& getElement(short idx);
	void updateEle(const int index, const cacheEle& pData);

private:
	RendererEle cache[RENDER_CACHE_SIZE+ CONST_CACHE_SIZE];
	LRUCache* _lru;
};