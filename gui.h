#ifndef SCABS_GUI_H
#define SCABS_GUI_H

#include "sdl_util.h"
#include "core.h"


#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240


typedef enum
{
	KEY_STATE_UNTOUCHED = 0,
	KEY_STATE_PRESSED,
	KEY_STATE_HELD,
	KEY_STATE_RELEASED,
	KEY_STATE_COUNT
} KeyState;


struct font
{
	int w;
	int h;
	SDL_Surface *map;
};


struct io
{
	SDL_Surface *screen;
	SDL_Surface *back;
	SDL_Surface *tile[TILE_COUNT][LETTER_COUNT];
	SDL_Surface *wild;
	SDL_Surface *lockon;
	struct font white_font;
	struct font black_font;
};


struct controls
{
	KeyState up;
	KeyState down;
	KeyState left;
	KeyState right;
	KeyState a;
	KeyState b;
	KeyState x;
	KeyState y;
};


struct env
{
	struct io io;
	struct game game;
	struct controls controls;
};


int scabs();


#endif


