#pragma once

#define RENDER_CACHE_SIZE (128)

enum eRendererState {
	eRenderEmpty,
	eRenderLoading,
	eRenderReady,
};
class RendererEle {
	int vbo;
	int texId;
	eRendererState state;
};


class renderCache
{
public:
	renderCache();
	~renderCache();

	short getFreeCacheIndex();
	void freeCache(short idx);

private:
	RendererEle cache[RENDER_CACHE_SIZE];
};