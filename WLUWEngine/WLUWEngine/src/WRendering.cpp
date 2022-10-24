#include "WRendering.h"

// Required includes
#include "Vector2.h"
#include "SDL.h"
#include "WWindow.h"

namespace WLUW
{
	WRenderer::WRenderer(WWindow &_renderWindow, Uint32 flags)
	{
		// Create and store the SDL Renderer
		renderer = SDL_CreateRenderer(_renderWindow.getWindow(), -1, flags);
		// Copy the pointer to the window. Can be used later
		renderWindow = &_renderWindow;
	}

	void WRenderer::present()
	{
		SDL_RenderPresent(renderer);
	}

	void WRenderer::clear(SDL_Color clearColor)
	{
		SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		SDL_RenderClear(renderer);
	}
	Vector2 WRenderer::worldToScreenCoords(Vector2 worldCoords)
	{
		return Vector2();
	}
	bool WRenderer::isVisibleInWindow(Vector2 worldCoords, Vector2 size)
	{
		return false;
	}
}