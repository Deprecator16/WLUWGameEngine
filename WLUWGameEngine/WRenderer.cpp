#include "WRenderer.h"

WRenderer::WRenderer()
{
	renderer = nullptr;
}

WRenderer::WRenderer(SDL_Renderer* renderer) :
	renderer(renderer)
{
}

void WRenderer::renderAll()
{
	for (int i = 0; i < renderQueue.size(); i++)
	{
		renderToScreen(renderQueue.top());
		renderQueue.pop();
	}
}

SDL_Renderer* WRenderer::getRenderer()
{
	return renderer;
}

void WRenderer::renderToScreen(WObject* obj)
{
	obj->texture->renderToRect(renderer, obj->hitbox.getBox());
}