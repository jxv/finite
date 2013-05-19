#include "dbg.h"
#include "sdl_util.h"


void free_surface(SDL_Surface *s)
{
	if (s) {
		 SDL_FreeSurface(s);
	}
}


SDL_Surface *load_surface(const char *filename)
{
	/* Magenta represents transparency */
	SDL_Surface *s, *tmp;
	Uint32 a;
	NOT(filename);
	s = NULL;
	tmp = IMG_Load(filename);
	if (!tmp)
		return NULL;
	s = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
	if (!s)
		return NULL;
	a = SDL_MapRGB(s->format, 0xff, 0x00, 0xff);
	SDL_SetColorKey(s, SDL_SRCCOLORKEY, a);
	return s;
} 


void delay(int st, int et, int fps)
{
	int ms = (1000 / fps) - (et - st);
	if (ms > 0)
		SDL_Delay(ms);
}


void draw_surface(SDL_Surface *s0, SDL_Surface *s1, int x, int y)
{
	SDL_Rect offset;
	NOT(s0), NOT(s1);
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface(s1, NULL, s0, &offset);
}


SDL_Surface *cpy_surface(SDL_Surface *s)
{
	SDL_Surface *cpy;
	NOT(s);	
	cpy = SDL_CreateRGBSurface(0, s->w, s->h, SCREEN_BPP, 0, 0, 0, 0);
	draw_surface(cpy,s,0,0);
	return cpy;
}


