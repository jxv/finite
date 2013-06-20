#ifndef FINITE_CORE_H
#define FINITE_CORE_H


#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>


#include "dbg.h"
#include "mem.h"


#define RES_PATH		"res/"


#define MAX(x, y)		(((x) > (y)) ? (x) : (y))
#define MIN(x, y)		(((x) < (y)) ? (x) : (y))
#define BOARD_X			15
#define BOARD_Y			15
#define BOARD_SIZE		MAX(BOARD_Y,BOARD_X)
#define RACK_SIZE		7
#define BAG_SIZE		(100 + 1)	/* needs a dummy element for impl. */
#define MAX_PLAYER		4


#define VALID_BOARD_X(x)	RANGE(x, 0, BOARD_X - 1)
#define VALID_BOARD_Y(y)	RANGE(y, 0, BOARD_Y - 1)
#define VALID_BOARD_SIZE(s)	RANGE(s, 0, BOARD_SIZE - 1)


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
	ADJUST_ERR_RACK_INVALID_TILE,
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


struct word
{
	int 		len;
	LetterType	letter[BOARD_SIZE];
};


struct tile
{
	TileType	type;
	LetterType	letter;
};


struct loc
{
	struct tile	tile;
	SqType		sq;
};


struct board
{
	struct tile	tile[BOARD_Y][BOARD_X];
	SqType		sq[BOARD_Y][BOARD_X];
	/* struct loc	loc[BOARD_Y][BOARD_X]; */
};


struct player
{
	bool		active;
	int		score;
	struct tile	tile[RACK_SIZE];
};


struct coor
{
	int		x;
	int		y;
};


struct movePlace
{
	int		num;
	int		rackIdx[RACK_SIZE];
	struct coor	coor[RACK_SIZE];
};


struct moveDiscard
{
	int		num;
	int		rackIdx[RACK_SIZE];
};


struct adjust
{
	AdjustType	type;
	int		playerIdx;
	union {
		int		rack[RACK_SIZE];
		AdjustErrType	err;
	} data;
};


struct move
{
	MoveType	type;
	int		playerIdx;
	union {
		struct movePlace	place;
		struct moveDiscard	discard;
	} data;
};


struct bag
{
	int		head;
	int		tail;
	struct tile	tile[BAG_SIZE];
};


struct dir
{
	DirType		type;
	int		x;
	int		y;
	int		len;
	int		pos[BOARD_SIZE];
};


struct path
{
	PathType	type;
	union {
		struct {
			struct dir	right;
			struct dir	down; 
		} dot;
		struct {
			struct dir	right;
			struct dir	down[BOARD_X];
		} horz;
		struct {
			struct dir	right[BOARD_Y];
			struct dir	down;
		} vert;
	} data;
	struct board board;
};


struct action
{
	ActionType	type;
	int		playerIdx;
	union {
		struct {
			int		score;
			int		num;
			int		rackIdx[RACK_SIZE];
			struct path	path;
		} place;
		struct moveDiscard	discard;
		ActionErrType		err;
	} data;
};


struct dict
{
	long		num;
	struct word	*words;
};


struct game
{
	int		turn;
	int		playerNum;
	struct player	player[MAX_PLAYER];
	struct board	board;
	struct bag	bag;
	struct dict	dict;
};


AdjustErrType	fdAdjustErr(struct adjust*, struct game*);
void		applyAdjust(struct game*, struct adjust*);
void		mkAction(struct action*, struct game*, struct move*);
bool		applyAction(struct game*, struct action*);
void		nextTurn(struct game*);
CmpType		cmpWord(struct word*, struct word*);
void		moveClr(struct move*);
void		actionClr(struct action*);
void		rmRackTile(struct player*, int*, int);
void		rackShift(struct player*);
void		rackRefill(struct player*, struct bag*);
bool		endGame(struct game *g);
int		fdWinner(struct game *g);


#endif


