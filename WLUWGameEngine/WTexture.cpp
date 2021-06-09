#include "WTexture.h"

WTexture::WTexture(WWindow* window, SDL_Renderer* renderer, int layer)
{
    this->window = window;
    this->renderer = renderer;

    texture = NULL;
    size.x = 0;
    size.y = 0;
    this->layer = layer;
}

WTexture::~WTexture()
{
    //Deallocate
    free();
}

bool WTexture::loadFromFile(string path)
{
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            //Get image dimensions
            size.x = loadedSurface->w;
            size.y = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    texture = newTexture;
    return texture != NULL;
}

void WTexture::free()
{
    //Free texture if it exists
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
        size.x = 0;
        size.y = 0;
    }
}

void WTexture::setAlpha(Uint8 alpha)
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(texture, alpha);
}

void WTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, size.x, size.y };

    //Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip);
}

Vector2 WTexture::getSize()
{
    return size;
}
