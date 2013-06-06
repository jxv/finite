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
	FOCUS_INVALID = -1,
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
	CMD_DROP,
	CMD_PLACE,
	CMD_GRAB,
	CMD_SWAP,
	CMD_CHOICE,
	CMD_COUNT
} cmd_t;


typedef enum
{
	TRANS_MOVE_INVALID = -1,
	TRANS_MOVE_NONE = 0,
	TRANS_MOVE_PLACE, 
	TRANS_MOVE_DISCARD,
	TRANS_MOVE_SKIP,
	TRANS_MOVE_QUIT,
	TRANS_MOVE_COUNT
} trans_move_t;


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
	bool			select;
	bool			cancel;
};


struct rackWidget
{
	int			focus;
	struct tileWidget	tileWidget[RACK_SIZE];
	bool			select;
	bool			cancel;
};


struct choiceWidget
{
	int			focus;
	bool			enabled[CHOICE_COUNT];
	bool			select;
	bool			cancel;
};


struct gameWidget
{
	focus_t			focus;
	bool			select;
	bool 			cancel;
	struct boardWidget	boardWidget;
	struct rackWidget	rackWidget;
	struct choiceWidget	choiceWidget;
};


struct cmd
{
	cmd_t			type;
	focus_t			focus;
	union {
		struct coor		board;
		int			rack;
		int			choice; 
	} data;
};


struct transTile
{
	bool 			has;
	int			rackId;
};

struct transMovePlace
{
	struct transTile	transTile;
	int 			num;
	int 			rackId[BOARD_Y][BOARD_X];
	struct coor		gridId[RACK_SIZE];
};


struct transMoveDiscard
{
	bool			tile[RACK_SIZE];
};


struct transMove
{
	trans_move_t		type;
	int			player_id;
	union {
		struct transMovePlace	place;
		struct transMoveDiscard discard;
	} data;
};


struct gui
{
	struct gameWidget	gameWidget;
	struct move		move;
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


