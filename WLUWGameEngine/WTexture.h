#pragma once

#include "Vector2.h"
#include "Header.h"

using namespace std;

class WTexture
{
public:
    // Constructor/Destructor functions
    WTexture(SDL_Window* window, SDL_Renderer* renderer);
    ~WTexture();

    // Management functions
    bool loadFromFile(std::string path);
    void free();
    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void setAlpha(Uint8 alpha);

    // Information functions
    Vector2 getSize();

private:
    // The actual hardware texture
    SDL_Texture* texture;

    // Image dimensions
    Vector2 size;

    // Pointers
    SDL_Window* window;
    SDL_Renderer* renderer;
};

