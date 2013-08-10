#ifndef FINITE_CORE_H
#define FINITE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dbg.h"
#include "mem.h"

#define RES_PATH "res/"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define BOARD_X	15
#define BOARD_Y	15
#define BOARD_SIZE MAX(BOARD_Y,BOARD_X)
#define RACK_SIZE 5
#define BAG_SIZE (100 + 1)	/* needs a dummy element */
#define MAX_PLAYER 4

#define VALID_BOARD_X(x) RANGE(x, 0, BOARD_X - 1)
#define VALID_BOARD_Y(y) RANGE(y, 0, BOARD_Y - 1)
#define VALID_BOARD_SIZE(s) RANGE(s, 0, BOARD_SIZE - 1)
#define VALID_RACK_SIZE(r) RANGE(r, 0, RACK_SIZE - 1)

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
	PLAYER_HUMAN = 0,
	PLAYER_AI,
	PLAYER_COUNT
} PlayerType;

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
	ACTION_ERR_UNKNOWN,
	ACTION_ERR_PLACE_OUT_OF_RANGE,
	ACTION_ERR_PLACE_SELF_OVERLAP,
	ACTION_ERR_PLACE_BOARD_OVERLAP,
	ACTION_ERR_PLACE_INVALID_RACK_ID,
	ACTION_ERR_PLACE_INVALID_SQ,
	ACTION_ERR_PLACE_NO_RACK,
	ACTION_ERR_PLACE_NO_DIR,
	ACTION_ERR_PLACE_INVALID_PATH,
	ACTION_ERR_PLACE_INVALID_WORD,
	ACTION_ERR_DISCARD_RULE,
	ACTION_ERR_SKIP_RULE,
	ACTION_ERR_QUIT_RULE,
	ACTION_ERR_COUNT
} ActionErrType;

typedef enum
{
	DIR_ERR_NONE = 0,
	DIR_ERR_NON_CONTINUOUS,
	DIR_ERR_INVALID_WORD,
	DIR_ERR_COUNT
} DirErrType;

typedef enum
{
	PATH_ERR_NONE = 0,
	PATH_ERR_NON_CONT,
	PATH_ERR_INVALID_WORD,
	PATH_ERR_INVALID_RULE,
	PATH_ERR_INVALID_PATH,
	PATH_ERR_COUNT
} PathErrType;

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


struct Game;

typedef struct Word
{
	int len;
	LetterType letter[BOARD_SIZE];
} Word;

typedef struct Tile
{
	TileType type;
	LetterType letter;
} Tile;

#define VALID_TILE_TYPE(tt) (RANGE(tt, 0, TILE_COUNT - 1))
#define VALID_LETTER_TYPE(lt) (RANGE(lt, 0, LETTER_COUNT - 1))
#define VALID_TILE(t) do { \
			VALID_TILE_TYPE((t).type); \
			if ((t).type != TILE_NONE) { \
				VALID_LETTER_TYPE((t).letter); \
			} \
		} while(0)

typedef struct Loc
{
	struct Tile tile;
	SqType sq;
} Loc;

typedef struct Board
{
	struct Tile tile[BOARD_Y][BOARD_X];
	SqType sq[BOARD_Y][BOARD_X];
	/* struct Loc loc[BOARD_Y][BOARD_X]; */
} Board;

typedef struct Player
{
	PlayerType type;
	bool active;
	int score;
	struct Tile tile[RACK_SIZE];
} Player;

typedef struct Coor
{
	int x;
	int y;
} Coor;

typedef struct MovePlace
{
	int num;
	int rackIdx[RACK_SIZE];
	struct Coor coor[RACK_SIZE];
} MovePlace;

typedef struct MoveDiscard
{
	int num;
	int rackIdx[RACK_SIZE];
} MoveDiscard;

typedef struct TileAdjust
{
	TileType type;
	int idx;
} TileAdjust;

typedef struct Adjust
{
	AdjustType type;
	union {
	struct TileAdjust tile[RACK_SIZE];
	AdjustErrType err;
	} data;
} Adjust;

typedef struct Move
{
	MoveType type;
	int playerIdx;
	union {
	struct MovePlace place;
	struct MoveDiscard discard;
	} data;
} Move;

typedef struct Bag
{
	int head;
	int tail;
	struct Tile tile[BAG_SIZE];
} Bag;

typedef struct Dir
{
	DirType type;
	int x;
	int y;
	int len;
	bool pos[BOARD_SIZE];
} Dir;

typedef struct Path
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
} Path;

typedef struct Action
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
} Action;

typedef struct Dict
{
	long num;
	struct Word *words;
} Dict;

typedef struct Rule
{
	bool (*place)(Word *, PathType, DirType);
	bool (*discard)(struct Game *, MoveDiscard *);
	bool (*skip)(struct Game *);
	bool (*quit)(struct Game *);
} Rule;

typedef struct Game
{
	int turn;
	int playerNum;
	struct Player player[MAX_PLAYER];
	struct Board board;
	struct Bag bag;
	struct Dict dict;
	struct Rule rule;
} Game;

void mkAdjust(Adjust *, Player *);
void adjustSwap(Adjust *, int, int);
AdjustErrType fdAdjustErr(Adjust *, Player *);
void applyAdjust(Player *, Adjust *);
void mkAction(Action *, Game *, Move *);
bool applyAction(Game *, Action *);
void nextTurn(Game *);
CmpType cmpWord(Word *, Word *);
void moveClr(Move *);
void actionClr(Action *);
void rmRackTile(Player *, int *, int);
void rackShift(Player *);
void rackRefill(Player *, Bag *);
bool endGame(Game *);
int fdWinner( Game *);
bool validRackIdx(int);
bool validBoardIdx(Coor);
int rackCount(Player *);
int adjustTileCount(Adjust *);
bool wordValid(Word *, Dict *);
bool boardEmpty(Board *b);

#endif

