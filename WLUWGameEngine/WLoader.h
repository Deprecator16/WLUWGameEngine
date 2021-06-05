#pragma once
#include "Header.h"
#include "WRenderer.h"
#include "WTexture.h"

class WLoader
{
public:
	
    static void init(WRenderer* renderer);

	// Load texture
	static bool loadTextureFromFile(WTexture* texture, std::string path);

private:
	// Renderer
	static WRenderer* renderer;
};