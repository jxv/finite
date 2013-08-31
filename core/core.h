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
#define RACK_SIZE 7
#define BAG_SIZE (100 + 1)	/* needs a dummy element */
#define MAX_PLAYER 4

#define VALID_BOARD_X(x) RANGE(x, 0, BOARD_X - 1)
#define VALID_BOARD_Y(y) RANGE(y, 0, BOARD_Y - 1)
#define VALID_BOARD_SIZE(s) RANGE(s, 0, BOARD_SIZE - 1)
#define VALID_RACK_SIZE(r) RANGE(r, 0, RACK_SIZE - 1)

typedef enum
{
	tileNone = -1,
	tileWild = 0,
	tileLetter,
	tileCount
} TileType;

typedef enum
{
	sqNormal = 0,
	sqDblLet,
	sqDblWrd,
	sqTrpLet,
	sqTrpWrd,
	sqFree,
	sqMystery,
	sqNoVowel,
	sqBlock,
	sqCount
} SqType;

typedef enum
{
	letterInvalid = -1,
	letterA = 0,
	letterB,
	letterC,
	letterD,
	letterE,
	letterF,
	letterG,
	letterH,
	letterI,
	letterJ,
	letterK,
	letterL,
	letterM,
	letterN,
	letterO,
	letterP,
	letterQ,
	letterR,
	letterS,
	letterT, 
	letterU,
	letterV,
	letterW,
	letterX,
	letterY,
	letterZ,
	letterCount
} LetterType;

typedef enum
{
	playerHuman = 0,
	playerAI,
	playerCount
} PlayerType;

typedef enum
{
	adjustInvalid = -1,
	adjustRack = 0,
	adjustCount
} AdjustType;

typedef enum
{
	adjustErrNone = 0,
	adjustErrRackOutOfRange,
	adjustErrRackDuplicateIdx,
	adjustErrCount
} AdjustErrType;

typedef enum
{
	moveInvalid = -1,
	movePlace = 0,
	moveDiscard,
	moveSkip,
	moveQuit,
	moveCount
} MoveType;

typedef enum
{
	actionErrNone = 0,
	actionErrUnknown,
	actionErrDiscardEmpty,
	actionErrPlaceOutOfRange,
	actionErrPlaceSelfOverlap,
	actionErrPlaceBoardOverlap,
	actionErrPlaceInvalidRackId,
	actionErrPlaceInvalidSq,
	actionErrPlaceNoRack,
	actionErrPlaceNoDir,
	actionErrPlaceInvalidPath,
	actionErrPlaceInvalidWord,
	actionErrDiscardRule,
	actionErrSkipRule,
	actionErrQuitRule,
	actionErrCount
} ActionErrType;

typedef enum
{
	dirErrNone = 0,
	dirErrNonContinuous,
	dirErrInvalidWord,
	dirErrCount
} DirErrType;

typedef enum
{
	pathErrNone = 0,
	pathErrNonCont,
	pathErrInvalidWord,
	pathErrInvalidRule,
	pathErrInvalidPath,
	pathErrCount
} PathErrType;

typedef enum
{
	dirInvalid = -1,
	dirRight = 0,
	dirDown,
	dirCount
} DirType;

typedef enum
{
	pathInvalid = -1,
	pathDot = 0,
	pathHorz,
	pathVert,
	pathCount
} PathType;

typedef enum
{
	cmpLess = -1,
	cmpEqual = 0,
	cmpGreater = 1
} CmpType;

typedef enum
{
	actionInvalid = -1,
	actionPlace = 0,
	actionDiscard,
	actionSkip,
	actionQuit,
	actionCount
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

#define VALID_TILE_TYPE(tt) (RANGE(tt, (-1), tileCount - 1))
#define VALID_LETTER_TYPE(lt) (RANGE(lt, 0, letterCount - 1))

#ifdef DEBUG
#define VALID_TILE(t) do { \
			VALID_TILE_TYPE((t).type); \
			if ((t).type != tileLetter) { \
				VALID_LETTER_TYPE((t).letter); \
			} \
		} while(0)
#else
#define VALID_TILE(t) NULL
#endif

#ifdef DEBUG
#define VALID_TILES(p) do {	\
			int i;	\
			for (i = 0; i < RACK_SIZE; i++) {	\
				VALID_TILE((p).tile[i]);	\
			}	\
		} while (0)
#else
#define VALID_TILES(t) NULL
#endif

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

typedef struct ActionPlace
{
	int score;
 	int num;
	int rackIdx[RACK_SIZE];
 	struct Path path;
} ActionPlace;

typedef struct ActionDiscard
{
	int score;
	int num;
	int rackIdx[RACK_SIZE];
} ActionDiscard;

typedef struct Action
{
	ActionType	type;
	int playerIdx;
	union {
	ActionPlace place;
	ActionDiscard discard;
	ActionErrType err;
	} data;
} Action;

struct Game;

typedef struct AiShare
{
	struct Game *game;
	Action action;
	float loading;
	bool shareStart;
	bool shareEnd;
	int difficulty;
} AiShare;

typedef struct Player
{
	PlayerType type;
	int rackSize;
	bool active;
	int score;
	struct Tile tile[RACK_SIZE];
	AiShare aiShare;
} Player;

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
	int rackSize;
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

bool bagEmpty(Bag *b);
int bagCount(Bag *b);
int bagSize(Bag *b);

int tileScore(Tile *t);

bool vowel(LetterType);
bool constant(LetterType);

#endif

