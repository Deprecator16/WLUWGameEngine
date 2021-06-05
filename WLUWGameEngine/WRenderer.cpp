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

/*bool WRenderer::sortTexture(WObject* obj1, WObject* obj2)
{
	return obj1->texture->layer < obj2->texture->layer;
}*/

void WRenderer::renderToScreen(WObject* obj)
{
	obj->texture->render(renderer, obj->pos.x, obj->pos.y);
}