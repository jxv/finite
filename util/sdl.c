#include "dbg.h"
#include "sdl.h"

void surfaceFree(SDL_Surface *s)
{
	if (s) {
		 SDL_FreeSurface(s);
	}
}

SDL_Surface *surfaceLoad(const char *filename)
{
	SDL_Surface *s, *tmp;
	Uint32 a;

	NOT(filename);

	s = NULL;
	tmp = IMG_Load(filename);
	if (!tmp) {
		return NULL;
	}
	s = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
	if (!s) {
		return NULL;
	}
	a = SDL_MapRGB(s->format, ALPHA_R, ALPHA_G, ALPHA_B);
	SDL_SetColorKey(s, SDL_SRCCOLORKEY, a);
	return s;
} 

void delay(int st, int et, int fps)
{
	int ms;

	assert(et >= st);
	assert(fps > 0);
	
	ms = (1000 / fps) - (et - st);
	if (ms > 0) {
		SDL_Delay(ms);
	}
}

void surfaceDraw(SDL_Surface *s0, SDL_Surface *s1, int x, int y)
{
	SDL_Rect offset;

	NOT(s0), NOT(s1);

	offset.x = x;
	offset.y = y;
	SDL_BlitSurface(s1, NULL, s0, &offset);
}

SDL_Surface *surfaceCpy(SDL_Surface *s)
{
	SDL_Surface *cpy;

	NOT(s);	

	cpy = SDL_CreateRGBSurface(0, s->w, s->h, SCREEN_BPP, 0, 0, 0, 0);
	surfaceDraw(cpy, s, 0, 0);
	return cpy;
}


