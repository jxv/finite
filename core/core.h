#ifndef FINITE_CORE_H
#define FINITE_CORE_H

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "dbg.h"
#include "mem.h"

#define RES_PATH "res/"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define BOARD_X	15
#define BOARD_Y	15
#define BOARD_SIZE MAX(BOARD_Y,BOARD_X)
#define RACK_SIZE 7
#define BAG_SIZE (100 + 1)	/* needs a dummy element for impl. */
#define MAX_PLAYER 4

#define VALID_BOARD_X(x) RANGE(x, 0, BOARD_X - 1)
#define VALID_BOARD_Y(y) RANGE(y, 0, BOARD_Y - 1)
#define VALID_BOARD_SIZE(s) RANGE(s, 0, BOARD_SIZE - 1)

typedef enum
{
	false = 0,
	true = 1
} bool;

typedef enum
{
	TILE_NONE = -1,
	TILE_WILD = 0,
	TILE_LETTER,
	TILE_COUNT
} TileType;

typedef enum
{
	SQ_NORMAL = 0,
	SQ_DBL_LET,
	SQ_DBL_WRD,
	SQ_TRP_LET,
	SQ_TRP_WRD,
	SQ_FREE,
	SQ_COUNT
} SqType;

typedef enum
{
	LETTER_INVALID = -1,
	LETTER_A = 0,
	LETTER_B,
	LETTER_C,
	LETTER_D,
	LETTER_E,
	LETTER_F,
	LETTER_G,
	LETTER_H,
	LETTER_I,
	LETTER_J,
	LETTER_K,
	LETTER_L,
	LETTER_M,
	LETTER_N,
	LETTER_O,
	LETTER_P,
	LETTER_Q,
	LETTER_R,
	LETTER_S,
	LETTER_T, 
	LETTER_U,
	LETTER_V,
	LETTER_W,
	LETTER_X,
	LETTER_Y,
	LETTER_Z,
	LETTER_COUNT
} LetterType;

typedef enum
{
	ADJUST_INVALID = -1,
	ADJUST_RACK = 0,
	ADJUST_COUNT
} AdjustType;

typedef enum
{
	ADJUST_ERR_NONE = 0,
	ADJUST_ERR_RACK_OUT_OF_RANGE,
	ADJUST_ERR_RACK_DUPLICATE_INDEX,
	ADJUST_ERR_COUNT
} AdjustErrType;

typedef enum
{
	MOVE_INVALID = -1,
	MOVE_PLACE = 0,
	MOVE_DISCARD,
	MOVE_SKIP,
	MOVE_QUIT,
	MOVE_COUNT
} MoveType;

typedef enum
{
	ACTION_ERR_NONE = 0,
	ACTION_ERR_UKNOWN,
	ACTION_ERR_PLACE_OUT_OF_RANGE,
	ACTION_ERR_PLACE_SELF_OVERLAP,
	ACTION_ERR_PLACE_BOARD_OVERLAP,
	ACTION_ERR_PLACE_INVALID_RACK_ID,
	ACTION_ERR_PLACE_INVALID_SQ,
	ACTION_ERR_PLACE_NO_RACK,
	ACTION_ERR_PLACE_NO_DIR,
	ACTION_ERR_PLACE_INVALID_PATH,
	ACTION_ERR_COUNT
} ActionErrType;

typedef enum
{
	DIR_INVALID = -1,
	DIR_RIGHT = 0,
	DIR_DOWN,
	DIR_COUNT
} DirType;

typedef enum
{
	PATH_INVALID = -1,
	PATH_DOT = 0,
	PATH_HORZ,
	PATH_VERT,
	PATH_COUNT
} PathType;

typedef enum
{
	CMP_LESS = -1,
	CMP_EQUAL = 0,
	CMP_GREATER = 1
} CmpType;

typedef enum
{
	ACTION_INVALID = -1,
	ACTION_PLACE = 0,
	ACTION_DISCARD,
	ACTION_SKIP,
	ACTION_QUIT,
	ACTION_COUNT
} ActionType;

struct Word
{
	int len;
	LetterType letter[BOARD_SIZE];
};

struct Tile
{
	TileType type;
	LetterType letter;
};

struct Loc
{
	struct Tile tile;
	SqType sq;
};

struct Board
{
	struct Tile tile[BOARD_Y][BOARD_X];
	SqType sq[BOARD_Y][BOARD_X];
	/* struct Loc loc[BOARD_Y][BOARD_X]; */
};

struct Player
{
	bool active;
	int score;
	struct Tile tile[RACK_SIZE];
};

struct Coor
{
	int x;
	int y;
};

struct MovePlace
{
	int num;
	int rackIdx[RACK_SIZE];
	struct Coor coor[RACK_SIZE];
};

struct MoveDiscard
{
	int num;
	int rackIdx[RACK_SIZE];
};

struct TileAdjust
{
	TileType type;
	int idx;
};

struct Adjust
{
	AdjustType type;
	union {
	struct TileAdjust tile[RACK_SIZE];
	AdjustErrType err;
	} data;
};

struct Move
{
	MoveType type;
	int playerIdx;
	union {
	struct MovePlace place;
	struct MoveDiscard discard;
	} data;
};

struct Bag
{
	int head;
	int tail;
	struct Tile tile[BAG_SIZE];
};

struct Dir
{
	DirType type;
	int x;
	int y;
	int len;
	int pos[BOARD_SIZE];
};

struct Path
{
	PathType type;
	struct Board board;
	union {
	struct {
	 	struct Dir right;
 		struct Dir down; 
	} dot;
	struct {
		struct Dir right;
	 	struct Dir down[BOARD_X];
	} horz;
	struct {
		struct Dir right[BOARD_Y];
 		struct Dir down;
	} vert;
	} data;
};

struct Action
{
	ActionType	type;
	int playerIdx;
	union {
	struct {
 		int score;
	 	int num;
 		int rackIdx[RACK_SIZE];
 		struct Path path;
	} place;
	struct MoveDiscard discard;
	ActionErrType err;
	} data;
};

struct Dict
{
	long num;
	struct Word *words;
};

struct Game
{
	int turn;
	int playerNum;
	struct Player player[MAX_PLAYER];
	struct Board board;
	struct Bag bag;
	struct Dict dict;
};

void mkAdjust(struct Adjust *, struct Player *);
void adjustSwap(struct Adjust *, int, int);
AdjustErrType fdAdjustErr(struct Adjust *, struct Player *);
void applyAdjust(struct Player *, struct Adjust *);
void mkAction(struct Action *, struct Game *, struct Move *);
bool applyAction(struct Game *, struct Action *);
void nextTurn(struct Game *);
CmpType cmpWord(struct Word *, struct Word *);
void moveClr(struct Move *);
void actionClr(struct Action *);
void rmRackTile(struct Player *, int *, int);
void rackShift(struct Player*);
void rackRefill(struct Player*, struct Bag *);
bool endGame(struct Game *);
int fdWinner(struct Game *);
bool validRackIdx(int);
bool validBoardIdx(struct Coor);

#endif


