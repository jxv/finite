#ifndef	SCABS_SDL_UTIL_H
#define	SCABS_SDL_UTIL_H


#include <SDL.h>
#include <SDL_image.h>


#define SCREEN_BPP	32


void free_surface(SDL_Surface *s);
SDL_Surface *load_surface(const char *filename);
void delay(int st, int et, int fps);
void draw_surface(SDL_Surface *s0, SDL_Surface *s1, int x, int y);
SDL_Surface *cpy_surface(SDL_Surface *s);


#endif


