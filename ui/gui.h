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
	FOCUS_BOARD = 0,
	FOCUS_RACK,
	FOCUS_CHOICE,
	FOCUS_COUNT
} focus_t;


typedef enum
{
	CHOICE_INVALID = -1,
	CHOICE_PLAY = 0,
	CHOICE_RECALL,
	CHOICE_QUIT,
	CHOICE_COUNT
} choice_t;


typedef enum
{
	CMD_INVALID = -1,
	CMD_COUNT
} cmd_t;


struct font
{
	int			width;
	int			height;
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


struct keystate
{
	keystate_t		type;
	float			time;
};


struct controls
{
	struct keystate		up;
	struct keystate		down;
	struct keystate		left;
	struct keystate		right;
	struct keystate		a;
	struct keystate		b;
	struct keystate		x;
	struct keystate		y;
};


struct locWidget
{
	bool			enabled;
	struct tile		tile;
	sq_t			sq;
};


struct tileWidget
{
	bool			enabled;
	struct tile		tile;
};


struct boardWidget
{
	struct coor		focus;
	struct locWidget	locWidget[BOARD_Y][BOARD_X];
};


struct rackWidget
{
	int			focus;
	struct tileWidget	tileWidget[RACK_SIZE];
};


struct choiceWidget
{
	int			focus;
	bool			enabled[CHOICE_COUNT];
};


struct gameWidget
{
	focus_t			focus;
	struct boardWidget	boardWidget;
	struct rackWidget	rackWidget;
	struct choiceWidget	choiceWidget;
};


struct gui
{
	struct gameWidget	gameWidget;
};


struct cmd
{
	cmd_t			type;
};


#define CMD_QUEUE_SIZE	16		


struct cmdQueue
{
	int			size;
	struct cmd		cmd[CMD_QUEUE_SIZE];
};


struct env
{
	struct io		io;
	struct game		game;
	struct controls		controls;
	struct gui		gui;
};


int gui();


#endif


