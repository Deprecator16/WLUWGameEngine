#include "WTexture.h"
#include "WTexture.h"

WLUW::WTexture::WTexture()
{
    layer = 0;
}

WLUW::WTexture::WTexture(WWindow* window, SDL_Renderer* renderer, int layer)
{
    this->window = window;
    this->renderer = renderer;

    texture = NULL;
    size.x = 0;
    size.y = 0;
    this->layer = layer;
}

WLUW::WTexture::~WTexture()
{
    //Deallocate
    free();
}

bool WLUW::WTexture::loadFromFile(std::string path)
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

bool WLUW::WTexture::makeText(std::string textureText, SDL_Color textColor)
{
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (texture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            size.x = textSurface->w;
            size.y = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    //Return success
    return texture != NULL;
}

bool WLUW::WTexture::loadFont(std::string path, int size)
{
    font = TTF_OpenFont(path.c_str(), size);
    if (font == NULL)
    {
        printf("Failed to load font at %s! TTF Error: %s\n", path.c_str(), TTF_GetError());
        return false;
    }
    return true;
}

void WLUW::WTexture::free()
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

void WLUW::WTexture::setAlpha(Uint8 alpha)
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(texture, alpha);
}

void WLUW::WTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    //Modulate texture
    SDL_SetTextureColorMod(texture, red, green, blue);
}

void WLUW::WTexture::render(SDL_Renderer* renderer, int x, int y, float scale, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, size.x * scale, size.y * scale };

    //Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w * scale;
        renderQuad.h = clip->h * scale;
    }

    //Render to screen
    SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip);
}

void WLUW::WTexture::renderToRect(SDL_Renderer* renderer, SDL_Rect* dstrect, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    //Render to screen
    SDL_RenderCopyEx(renderer, texture, clip, dstrect, angle, center, flip);
}

WLUW::Vector2 WLUW::WTexture::getSize()
{
    return size;
}

