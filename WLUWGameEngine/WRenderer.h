#pragma once

#include <queue>
#include "Header.h"
#include "WObject.h"

class WRenderer
{
public:

	// Constructors
	WRenderer();
	WRenderer(SDL_Renderer* renderer);

	// Render all currently stored textures
	void renderAll();

	// [DEBUG] Add object to storage
	void addObj(WObject* obj)
	{
		renderQueue.push(obj);
	}

	// Returns renderer
	SDL_Renderer* getRenderer();

	// Camera
	void setCameraPos(Vector2 pos);

private:

	SDL_Renderer* renderer;

	// Texture storage and sorting
	//bool sortTexture(WObject* obj1, WObject* obj2);

	
	// Sort textures
	struct sortTexture
	{
		bool operator()(WObject* obj1, WObject* obj2)
		{
			return obj1->texture->layer < obj2->texture->layer;
		}
	};

	// Store textures in order of layer
	// Higher layers are rendered farther back
	priority_queue<WObject*, vector<WObject*>, sortTexture> renderQueue;

	// Render a single texture
	void renderToScreen(WObject* obj);

	// Camera Position
	Vector2 cameraPos;
};