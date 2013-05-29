#ifndef FINITE_GUI_H
#define FINITE_GUI_H

#include "sdl.h"
#include "common.h"


#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240


typedef enum
{
	KEYSTATE_UNTOUCHED = 0,
	KEYSTATE_PRESSED,
	KEYSTATE_HELD,
	KEYSTATE_RELEASED,
	KEYSTATE_COUNT
} keystate_t;


typedef enum
{
	SELECTION_BOARD = 0,
	SELECTION_RACK,
	SELECTION_CHOICE,
	SELECTION_COUNT
} selection_t;


typedef enum
{
	CHOICE_INVALID = -1,
	CHOICE_PLAY = 0,
	CHOICE_RECALL,
	CHOICE_QUIT,
	CHOICE_COUNT
} choice_t;


struct font
{
	int			w;
	int			h;
	SDL_Surface		*map;
};


struct io
{
	SDL_Surface		*screen;
	SDL_Surface		*back;
	SDL_Surface		*tile[TILE_COUNT][LETTER_COUNT];
	SDL_Surface		*wild;
	SDL_Surface		*lockon;
	struct font		white_font;
	struct font		black_font;
};


struct controls
{
	keystate_t		up;
	keystate_t		down;
	keystate_t		left;
	keystate_t		right;
	keystate_t		a;
	keystate_t		b;
	keystate_t		x;
	keystate_t		y;
};


struct selection
{
	selection_t		type;
	union {
		struct coor			board;
		int				rack;
		choice_t			choice;
	} data;
};


struct env
{
	struct io		io;
	struct game		game;
	struct controls		controls;
	struct selection	selection;
};


int gui();


#endif


