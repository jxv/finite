#ifndef FINITE_GUI_H
#define FINITE_GUI_H

#include "sdl.h"
#include "common.h"

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240

typedef enum
{
	KEY_STATE_UNTOUCHED = 0,
	KEY_STATE_PRESSED,
	KEY_STATE_HELD,
	KEY_STATE_RELEASED,
	KEY_STATE_COUNT
} KeyStateType;

typedef enum
{
	CHOICE_INVALID = -1,
	CHOICE_RECALL = 0,
	CHOICE_MODE,
	CHOICE_PLAY,
	CHOICE_COUNT
} ChoiceType;

typedef enum
{
	MODE_INVALID = -1,
	MODE_PLACE = 0,
	MODE_DISCARD,
	MODE_SKIP,
	MODE_COUNT
} ModeType;

typedef enum
{
	CMD_INVALID = -1,
	CMD_FOCUS_PREV = 0,
	CMD_FOCUS_NEXT,
	CMD_BOARD,
	CMD_RACK,
	CMD_RECALL,
	CMD_MODE_UP,
	CMD_MODE_DOWN,
	CMD_PLAY,
	CMD_BOARD_CANCEL,
	CMD_RACK_CANCEL,
	CMD_CHOICE_CANCEL,
	CMD_QUIT,
	CMD_COUNT
} CmdType;

typedef enum
{
	TRANS_MOVE_INVALID = -1,
	TRANS_MOVE_NONE = 0,
	TRANS_MOVE_PLACE_INIT,
	TRANS_MOVE_PLACE,
	TRANS_MOVE_PLACE_HOLD,
	TRANS_MOVE_PLACE_PLAY,
	TRANS_MOVE_DISCARD_INIT,
	TRANS_MOVE_DISCARD,
	TRANS_MOVE_DISCARD_PLAY,
	TRANS_MOVE_SKIP,
	TRANS_MOVE_SKIP_PLAY,
	TRANS_MOVE_QUIT,
	TRANS_MOVE_COUNT
} TransMoveType;

typedef enum
{
	TILE_LOOK_DISABLE = 0,
	TILE_LOOK_NORMAL,
	TILE_LOOK_HOLD,
	TILE_LOOK_COUNT
} TileLookType;

struct Font
{
	int width;
	int height;
	SDL_Surface *map;
};

struct IO
{
	SDL_Surface *screen;
	SDL_Surface *back;
	SDL_Surface *tile[TILE_COUNT][LETTER_COUNT][TILE_LOOK_COUNT];
	SDL_Surface *wild[TILE_LOOK_COUNT];
	SDL_Surface *lockon;
	SDL_Surface *recall;
	SDL_Surface *recallDisable;
	SDL_Surface *mode;
	SDL_Surface *modeDisable;
	SDL_Surface *place;
	SDL_Surface *placeDisable;
	SDL_Surface *discard;
	SDL_Surface *discardDisable;
	SDL_Surface *skip;
	SDL_Surface *skipDisable;
	SDL_Surface *play;
	SDL_Surface *playDisable;
	struct Font white_font;
	struct Font black_font;
};

struct KeyState
{
	KeyStateType type;
	float time;
};

struct Controls
{
	struct KeyState up;
	struct KeyState down;
	struct KeyState left;
	struct KeyState right;
	struct KeyState a;
	struct KeyState b;
	struct KeyState x;
	struct KeyState y;
	struct KeyState l;
	struct KeyState r;
};

struct Cmd
{
	CmdType type;
	union {
	struct Coor board;
	int rack;
	} data;
};

struct GridWidget
{
	struct Coor index;
	bool **button;
	int width;
	int height;
};

typedef enum
{
	GUI_FOCUS_INVALID = -1,
	GUI_FOCUS_BOARD = 0,
	GUI_FOCUS_RACK,
	GUI_FOCUS_CHOICE,
	GUI_FOCUS_COUNT
} GUIFocusType;

struct GameGUI
{
	GUIFocusType focus;
	struct GridWidget rackWidget;
	struct GridWidget boardWidget;
	struct GridWidget choiceWidget;
};

struct GUI
{
	struct GameGUI gameGui;
};

struct MoveModePlace
{
	int num;
	int idx;
	bool taken[BOARD_Y][BOARD_X];
	int rackIdx[BOARD_Y][BOARD_X];
	struct Coor boardIdx[RACK_SIZE];
};

struct MoveModeDiscard
{
	int num;
	bool rack[RACK_SIZE];
};

struct TransMove
{
	TransMoveType type;
	int playerIdx;
	struct Adjust adjust;
	union {
	struct MoveModePlace place;
	struct MoveModeDiscard discard;
	} data;
};

struct Env
{
	struct IO io;
	struct Game game;
	struct Controls controls;
	struct GUI gui;
	struct TransMove transMove;
};

int gui();

#endif

