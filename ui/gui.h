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
} KeystateType;


typedef enum
{
	FOCUS_INVALID = -1,
	FOCUS_BOARD = 0,
	FOCUS_RACK,
	FOCUS_CHOICE,
	FOCUS_COUNT
} FocusType;


typedef enum
{
	CHOICE_INVALID = -1,
	CHOICE_MODE = 0,
	CHOICE_PLAY,
	CHOICE_RECALL,
	CHOICE_QUIT,
	CHOICE_COUNT
} ChoiceType;


typedef enum
{
	CMD_INVALID = -1,
	CMD_DROP,
	CMD_PLACE,
	CMD_GRAB,
	CMD_SWAP,
	CMD_DISCARD,
	CMD_KEEP,
	CMD_MODE,
	CMD_PLAY,
	CMD_RECALL,
	CMD_QUIT,
	CMD_COUNT
} CmdType;


typedef enum
{
	TRANS_MOVE_INVALID = -1,
	TRANS_MOVE_NONE = 0,
	TRANS_MOVE_PLACE, 
	TRANS_MOVE_DISCARD,
	TRANS_MOVE_SKIP,
	TRANS_MOVE_QUIT,
	TRANS_MOVE_COUNT
} TransMoveType;


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
	SDL_Surface		*recall;
	SDL_Surface		*mode;
	SDL_Surface		*place;
	SDL_Surface		*discard;
	SDL_Surface		*skip;
	SDL_Surface		*play;
	struct font		white_font;
	struct font		black_font;
};


struct keystate
{
	KeystateType		type;
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


struct gridWidget
{
	struct coor		index;
	bool			**button;
	void			(*onPress)(void*, struct coor);
	int			width;
	int			height;
};


typedef enum
{
	GUI_FOCUS_INVALID = -1,
	GUI_FOCUS_RACK = 0,
	GUI_FOCUS_BOARD,
	GUI_FOCUS_CHOICE,
	GUI_FOCUS_COUNT
} gui_FocusType;


struct gameGui
{
	gui_FocusType		focus;
	struct gridWidget	rackWidget;
	struct gridWidget	boardWidget;
	struct gridWidget	choiceWidget;
};


struct gui
{
	struct gameGui		gameGui;
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


