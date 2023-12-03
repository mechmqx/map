#include "renderer_cache.h"



short renderCache::getFreeCacheIndex() {
	return 0;
}

void renderCache::freeCache(short idx) {

}

RendererEle& renderCache::getElement(short idx) {
	return cache[idx];
}

renderCache::renderCache()
{
}

renderCache::~renderCache()
{
}
