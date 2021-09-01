#pragma once

#include "Vector2.h"
#include "WWindow.h"
#include "Header.h"

using namespace std;

class WTexture
{
public:
    // Constructor/Destructor
    WTexture();
    WTexture(WWindow* window, SDL_Renderer* renderer, int layer = 0);
    ~WTexture();

    // Management functions
    bool loadFromFile(std::string path);
    void free();
    void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void renderToRect(SDL_Renderer* renderer, SDL_Rect* dstrect, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void setAlpha(Uint8 alpha);

    // Information functions
    Vector2 getSize();

    // The actual hardware texture
    SDL_Texture* texture;

    // Image dimensions
    Vector2 size;

    // Layer to render texture on
    int layer;

private:
    // Pointers
    WWindow* window;
    SDL_Renderer* renderer;
};

