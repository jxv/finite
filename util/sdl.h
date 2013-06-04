#ifndef	FINITE_SDL_UTIL_H
#define	FINITE_SDL_UTIL_H


#include <SDL.h>
#include <SDL_image.h>


#define SCREEN_BPP	32


void surfaceFree(SDL_Surface *s);
SDL_Surface *surfaceLoad(const char *filename);
void delay(int st, int et, int fps);
void surfaceDraw(SDL_Surface *s0, SDL_Surface *s1, int x, int y);
SDL_Surface *surfaceCpy(SDL_Surface *s);


#endif


