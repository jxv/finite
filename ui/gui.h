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
	CHOICE_SHUFFLE,
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
	CMD_FOCUS_TOP = 0,
	CMD_FOCUS_BOTTOM,
	CMD_BOARD_SELECT,
	CMD_RACK_SELECT,
	CMD_BOARD,
	CMD_BOARD_LEFT,
	CMD_BOARD_RIGHT,
	CMD_BOARD_UP,
	CMD_BOARD_DOWN,
	CMD_RACK,
	CMD_RACK_LEFT,
	CMD_RACK_RIGHT,
	CMD_CHOICE,
	CMD_CHOICE_LEFT,
	CMD_CHOICE_RIGHT,
	CMD_RECALL,
	CMD_MODE_UP,
	CMD_MODE_DOWN,
	CMD_PLAY,
	CMD_SHUFFLE,
	CMD_BOARD_CANCEL,
	CMD_RACK_CANCEL,
	CMD_CHOICE_CANCEL,
	CMD_TILE_PREV,
	CMD_TILE_NEXT,
	CMD_QUIT,
	CMD_COUNT
} CmdType;

typedef enum
{
	TRANS_MOVE_INVALID = -1,
	TRANS_MOVE_NONE = 0,
	TRANS_MOVE_PLACE,
	TRANS_MOVE_PLACE_WILD,
	TRANS_MOVE_PLACE_END,
	TRANS_MOVE_PLACE_PLAY,
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
	TILE_LOOK_GHOST,
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
	float time;
	SDL_Surface *screen;
	SDL_Surface *back;
	SDL_Surface *tile[TILE_COUNT][LETTER_COUNT][TILE_LOOK_COUNT];
	SDL_Surface *wild[TILE_LOOK_COUNT];
	SDL_Surface *wildUp;
	SDL_Surface *wildDown;
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
	SDL_Surface *shuffle;
	SDL_Surface *shuffleDisable;
	SDL_Surface *sq[SQ_COUNT];
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
	int choice;
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
	GUIFocusType bottomLast;
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

#define TILE_WIDTH 14
#define TILE_HEIGHT 14

#endif

