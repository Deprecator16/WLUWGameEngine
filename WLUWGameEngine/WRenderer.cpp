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

void WRenderer::setCameraPos(Vector2 pos)
{
	cameraPos = pos;
}

void WRenderer::renderToScreen(WObject* obj)
{
	// change box position according to camera
	SDL_Rect rect = *obj->hitbox.getBox();
	rect.x -= cameraPos.x;
	rect.y -= cameraPos.y;

	obj->texture->renderToRect(renderer, &rect);
}