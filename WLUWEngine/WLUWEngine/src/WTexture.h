#pragma once

#include <string>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "Vector2.h"
#include "WWindow.h"

namespace WLUW
{
    class WTexture
    {
    public:
        // Constructor/Destructor
        WTexture();
        WTexture(WWindow* window, SDL_Renderer* renderer, int layer = 0);
        ~WTexture();

        // Management functions
        bool loadFromFile(std::string path);
        bool makeText(std::string textureText, SDL_Color textColor);
        bool loadFont(std::string path, int size);

        // Destruction function
        void free();

        // Rendering functions
        void render(SDL_Renderer* renderer, int x, int y, float scale, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        void renderToRect(SDL_Renderer* renderer, SDL_Rect* dstrect, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        // Manipulation functions
        void setColor(Uint8 red, Uint8 green, Uint8 blue);
        void setAlpha(Uint8 alpha);

        // Information functions
        Vector2 getSize();

        // The actual hardware texture
        SDL_Texture* texture;
        TTF_Font* font;

        // Image dimensions
        Vector2 size;

        // Layer to render texture on
        int layer;

    private:
        // Pointers
        WWindow* window;
        SDL_Renderer* renderer;
    };
}

