#pragma once
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#ifdef _DEBUG
#pragma comment (lib, "WLUWEngine.lib")
#pragma comment (lib, "SDL2-2.0.14/lib/x64/SDL2.lib")
#pragma comment (lib, "SDL2_ttf-2.0.15/lib/x64/SDL2_ttf.lib")
#pragma comment (lib, "SDL2_mixer-2.0.4/lib/x64/SDL2_mixer.lib")
#pragma comment (lib, "SDL2_image-2.0.5/lib/x64/SDL2_image.lib")
#else
#pragma comment (lib, "WLUWEngine.lib")
#pragma comment (lib, "SDL2-2.0.14/lib/x64/SDL2.lib")
#pragma comment (lib, "SDL2_ttf-2.0.15/lib/x64/SDL2_ttf.lib")
#pragma comment (lib, "SDL2_mixer-2.0.4/lib/x64/SDL2_mixer.lib")
#pragma comment (lib, "SDL2_image-2.0.5/lib/x64/SDL2_image.lib")
#endif