#include "config.h"
#include "renderer_cache.h"
#include "esUtil.h"

static char dummy_data[IMG_SIZE * IMG_SIZE *3] = { 0 };

int renderCache::getFreeCacheIndex(int key, int& oldkey) 
{
	sIRUState state = { eIRUFresh,-1 };
	int idx = _lru->get(key, state);

	oldkey = state.oldKey;
	
	return idx;
}

void renderCache::freeCache(short idx) {

}

RendererEle& renderCache::getElement(short idx) {
	return cache[idx];
}

void renderCache::updateEle(const int index, const cacheEle& pData) {

	auto& ele = cache[index];
	ele.state = eRenderLoading;
	glBindBuffer(GL_ARRAY_BUFFER, ele.vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, TILE_V_NUM * 2 * 4, pData.vert);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, ele.texId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLsizei)IMG_SIZE,
		(GLsizei)IMG_SIZE, GL_RGB,
		GL_UNSIGNED_BYTE, pData.image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFlush();
	ele.state = eRenderReady;
}

renderCache::renderCache()
{
	_lru = new LRUCache(RENDER_CACHE_SIZE);

	// init render pool
	for (int i = 0; i < RENDER_CACHE_SIZE+ CONST_CACHE_SIZE; i++) {
		auto& ele = cache[i];

		glGenBuffers(1, (GLuint*)&ele.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, ele.vbo);
		glBufferData(GL_ARRAY_BUFFER, TILE_V_NUM * 2 * 4, dummy_data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenTextures(1, (GLuint*)&ele.texId);
		glBindTexture(GL_TEXTURE_2D, ele.texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			(GLsizei)IMG_SIZE,
			(GLsizei)IMG_SIZE,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			(GLvoid*)dummy_data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glFlush();
}

renderCache::~renderCache()
{
	delete _lru;
	for (int i = 0; i < RENDER_CACHE_SIZE; i++)
	{
		auto& ele = cache[i];
		glDeleteBuffers(1, (GLuint*)&ele.vbo);
		glDeleteTextures(1, (GLuint*)&ele.texId);
	}
}
