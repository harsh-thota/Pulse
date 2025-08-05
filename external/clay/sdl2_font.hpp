#pragma once
#include <SDL_ttf.h>

struct SDL2_Font
{
	uint32_t id;
	TTF_Font* font;
};