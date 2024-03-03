#pragma once
#include "lru_cache.h"
#include "data_cache.h"
#include "tile_id.h"

#define RENDER_CACHE_SIZE (128)
#define CONST_CACHE_SIZE (8)

typedef enum eRendererState {
	eRenderEmpty,
	eRenderUploading,
	eRenderReady,
}eRendererState;
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

	void updateCacheIndex(tileId& id);
	int getFreeCacheIndex(tileId& id, tileId& oldid);

	RendererEle& getElement(short idx);
	void updateEle(const int index, const cacheEle& pData);

private:
	RendererEle cache[RENDER_CACHE_SIZE+ CONST_CACHE_SIZE];
	LRUCache* _lru;
};