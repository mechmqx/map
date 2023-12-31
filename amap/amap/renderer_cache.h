#pragma once
#include "lru_cache.h"
#include "data_cache.h"

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
};


class renderCache
{
public:
	renderCache();
	~renderCache();

	int getFreeCacheIndex(int key, int& oldkey);
	void freeCache(short idx);

	RendererEle& getElement(short idx);
	void updateEle(const int index, const cacheEle& pData);

private:
	RendererEle cache[RENDER_CACHE_SIZE+ CONST_CACHE_SIZE];
	LRUCache* _lru;
};