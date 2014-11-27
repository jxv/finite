#include "dbg.h"
#include "sdl.h"

void surface_free(SDL_Surface *s)
{
	if (s)
		 SDL_FreeSurface(s);
}

SDL_Surface *surface_load(const char *filename)
{
	SDL_Surface *tmp = IMG_Load(filename);
	if (!tmp)
		return NULL;
	SDL_Surface *s = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
	if (!s)
		return NULL;
	Uint32 a = SDL_MapRGB(s->format, ALPHA_R, ALPHA_G, ALPHA_B);
	SDL_SetColorKey(s, SDL_SRCCOLORKEY, a);
	return s;
}

SDL_Surface *surface_alpha_load(const char *filename)
{
	SDL_Surface *tmp = IMG_Load(filename);
	if (!tmp)
		return NULL;
	SDL_Surface *s = SDL_DisplayFormatAlpha(tmp);
	SDL_FreeSurface(tmp);
	return s;
}

void delay(int st, int et, int fps)
{
	const int ms = (1000 / fps) - (et - st);
	if (ms > 0)
		SDL_Delay(ms);
}

void surface_draw(SDL_Surface *s0, SDL_Surface *s1, int x, int y)
{
	SDL_Rect offset = {.x = x, .y = y};
	SDL_BlitSurface(s1, NULL, s0, &offset);
}

SDL_Surface *surface_cpy(SDL_Surface *s)
{
	SDL_Surface *cpy = SDL_CreateRGBSurface(0, s->w, s->h, SCREEN_BPP, 0, 0,
						0, 0);
	surface_draw(cpy, s, 0, 0);
	return cpy;
}
