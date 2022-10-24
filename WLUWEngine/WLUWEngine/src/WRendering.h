#pragma once
#include "SDL_pixels.h"
#include "Vector2.h"

//struct SDL_Color;
struct SDL_Renderer;

namespace WLUW
{
	//class Vector2;
	class WWindow;

	typedef struct WRenderInfo
	{
		Vector2 worldCoords;
		Vector2 size;
		SDL_Color color; // Useful for rendering empty textures
	} WRenderInfo;

	class WRenderer
	{
	public:
		/**
		 * The Constructor for the WRenderer class
		 * 
		 * @param renderWindow The WWindow that this renderer will render to
		 * @param flags The SDL flags used in the setup of the WRenderer
		 */
		WRenderer(WWindow &renderWindow, Uint32 flags = 0);
		
		/**
		 * Presents all currently rendered objects
		 * Called once per frame
		 */
		void present();

		/**
		 * Clears the window a specified color. Defaults to white
		 * 
		 * @param clearColor The color to clear the window. Defaulted to white
		 */
		void clear(SDL_Color clearColor = SDL_Color({ 255, 255, 255, 255 }));

	protected:
		Vector2 worldToScreenCoords(Vector2 worldCoords);
		bool isVisibleInWindow(Vector2 worldCoords, Vector2 size);
		WWindow* renderWindow;
		SDL_Renderer* renderer;
	};
}