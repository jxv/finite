#ifndef FINITE_CORE_H
#define FINITE_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

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

enum tile_tag
{
        TILE_NONE = -1,
        TILE_WILD = 0,
        TILE_LETTER,
        TILE_COUNT,
};

typedef enum sq
{
        SQ_NORMAL = 0,
        SQ_DBL_LET,
        SQ_DBL_WRD,
        SQ_TRP_LET,
        SQ_TRP_WRD,
        SQ_FREE,
        SQ_MYSTERY,
        SQ_NO_VOWEL,
        SQ_BLOCK,
        SQ_COUNT,
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

typedef enum letter
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
        LETTER_COUNT,
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

typedef enum player_tag
{
        PLAYER_HUMAN = 0,
        PLAYER_AI,
        PLAYER_COUNT,
	playerHuman = 0,
	playerAI,
	playerCount
} PlayerType;

typedef enum adjust_tag
{
        ADJUST_INVALID = -1,
        ADJUST_RACK = 0,
        ADJUST_COUNT,
	adjustInvalid = -1,
	adjustRack = 0,
	adjustCount
} AdjustType;

typedef enum adjust_err
{
        ADJUST_ERR_NONE = 0,
        ADJUST_ERR_RACK_OUT_OF_RANGE,
        ADJUST_ERR_RACK_DUP_IDX,
        ADJUST_ERR_COUNT,
	adjustErrNone = 0,
	adjustErrRackOutOfRange,
	adjustErrRackDuplicateIdx,
	adjustErrCount
} AdjustErrType;

typedef enum move_tag
{
        MOVE_INVALID = -1,
        MOVE_PLACE,
        MOVE_DISCARD,
        MOVE_SKIP,
        MOVE_QUIT,
        MOVE_COUNT,
	moveInvalid = -1,
	movePlace = 0,
	moveDiscard,
	moveSkip,
	moveQuit,
	moveCount
} MoveType;

typedef enum action_err
{
        ACTION_ERR_NONE,
        ACTION_ERR_UNKNOWN,
        ACTION_ERR_DISCARD_EMPTY,
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
        ACTION_ERR_COUNT,
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

typedef enum dir_err_tag
{
        DIR_ERR_NONE = 0,
        DIR_ERR_NON_CONT,
        DIR_ERR_INVALID_WORD,
        DIR_ERR_COUNT,
	dirErrNone = 0,
	dirErrNonContinuous,
	dirErrInvalidWord,
	dirErrCount
} DirErrType;

typedef enum path_err_tag
{
        PATH_ERR_NONE = 0,
        PATH_ERR_NON_CONT,
        PATH_ERR_INVALID_WORD,
        PATH_ERR_INVALID_RULE,
        PATH_ERR_INVALID_PATH,
        PATH_ERR_COUNT,
	pathErrNone = 0,
	pathErrNonCont,
	pathErrInvalidWord,
	pathErrInvalidRule,
	pathErrInvalidPath,
	pathErrCount
} PathErrType;

typedef enum dir_tag
{
        DIR_INVALID = -1,
        DIR_RIGHT = 0,
        DIR_DOWN,
        DIR_COUNT,
	dirInvalid = -1,
	dirRight = 0,
	dirDown,
	dirCount
} DirType;

typedef enum path_tag
{
        PATH_INVALID = -1,
        PATH_DOT = 0,
        PATH_HORZ,
        PATH_VERT,
        PATH_COUNT,
	pathInvalid = -1,
	pathDot = 0,
	pathHorz,
	pathVert,
	pathCount
} PathType;

typedef enum cmp
{
        CMP_LESS = -1,
        CMP_EQUAL = 0,
        CMP_GREATER = 1,
	cmpLess = -1,
	cmpEqual = 0,
	cmpGreater = 1
} CmpType;

typedef enum action_tag
{
        ACTION_INVALID = -1,
        ACTION_PLACE = 0,
        ACTION_DISCARD,
        ACTION_SKIP,
        ACTION_QUIT,
        ACTION_COUNT,
	actionInvalid = -1,
	actionPlace = 0,
	actionDiscard,
	actionSkip,
	actionQuit,
	actionCount
} ActionType;


typedef enum tile_tag tile_tag_t;
typedef enum sq sq_t;
typedef enum letter letter_t;
typedef enum player_tag player_tag_t;
typedef enum adjust_tag adjust_tag_t;
typedef enum adjust_err adjust_err_t;
typedef enum move_tag move_tag_t;
typedef enum action_err action_err_t;
typedef enum dir_err_tag dir_err_tag_t;
typedef enum path_err_tag path_err_tag_t;
typedef enum dir_tag dir_tag_t;
typedef enum path_tag path_tag_t;
typedef enum cmp cmp_t;
typedef enum action_tag action_tag_t;

struct word;
struct tile;
struct loc;
struct board;
struct coor;
struct move_place;
struct move_discard;
struct tile_adjust;
struct adjust;
struct move;
struct bag;
struct dir;
struct path_dot;
struct path_horz;
struct path_vert;
struct path;
struct action_place;
struct action_discard;
struct action;
struct ai_share;
struct player;
struct dict;
struct rule;
struct game;

typedef struct word
{
	int len;
	letter_t letter[BOARD_SIZE];
} Word;

typedef struct tile
{
	tile_tag_t type;
	letter_t letter;
} Tile;

#define VALID_TILE_TYPE(tt) (RANGE(tt, (-1), TILE_COUNT - 1))
#define VALID_LETTER_TYPE(lt) (RANGE(lt, 0, LETTER_COUNT - 1))

#ifdef DEBUG
#define VALID_TILE(t) do { \
			VALID_TILE_TYPE((t).type); \
			if ((t).type != TILE_LETTER) { \
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

typedef struct loc
{
	struct tile tile;
	sq_t sq;
} Loc;

typedef struct board
{
	struct tile tile[BOARD_Y][BOARD_X];
	sq_t sq[BOARD_Y][BOARD_X];
	/* struct loc loc[BOARD_Y][BOARD_X]; */
} Board;

typedef struct coor
{
	int x;
	int y;
} Coor;

typedef struct move_place
{
	int num;
	int rackIdx[RACK_SIZE];
	struct coor coor[RACK_SIZE];
} MovePlace;

typedef struct move_discard
{
	int num;
	int rackIdx[RACK_SIZE];
} MoveDiscard;

typedef struct tile_adjust
{
	tile_tag_t type;
	int idx;
} TileAdjust;

typedef struct adjust
{
	adjust_tag_t type;
	union {
	        struct tile_adjust tile[RACK_SIZE];
        	adjust_err_t err;
	} data;
} Adjust;

typedef struct move
{
	MoveType type;
	int playerIdx;
	union {
        	struct move_place place;
        	struct move_discard discard;
	} data;
} Move;

typedef struct bag
{
	int head;
	int tail;
	struct tile tile[BAG_SIZE];
} Bag;

typedef struct dir
{
	dir_tag_t type;
	int x;
	int y;
	int len;
	bool pos[BOARD_SIZE];
} Dir;

struct path_dot 
{
 	struct dir right;
	struct dir down; 
};

struct path_horz
{
 	struct dir right;
	struct dir down[BOARD_X]; 
};

struct path_vert
{
 	struct dir right[BOARD_Y];
	struct dir down; 
};

typedef struct path
{
	path_tag_t type;
	struct board board;
	union {
                struct path_dot dot;
                struct path_horz horz;
                struct path_vert vert;
	} data;
} Path;

typedef struct action_place
{
	int score;
 	int num;
	int rackIdx[RACK_SIZE];
 	struct path path;
} ActionPlace;

typedef struct action_discard
{
	int score;
	int num;
	int rackIdx[RACK_SIZE];
} ActionDiscard;

typedef struct action
{
	action_tag_t type;
	int playerIdx;
	union {
	        struct action_place place;
        	struct action_discard discard;
	        action_err_t err;
	} data;
} Action;

typedef struct ai_share
{
	struct game *game;
	struct action action;
	float loading;
	bool shareStart;
	bool shareEnd;
	int difficulty;
} AiShare;

typedef struct player
{
	player_tag_t type;
	bool active;
	int score;
	struct tile tile[RACK_SIZE];
	struct ai_share aiShare;
} Player;

typedef struct dict
{
	long num;
	struct word *words;
} Dict;

typedef struct rule
{
	bool (*place)(const struct word *, path_tag_t, dir_tag_t);
	bool (*discard)(const struct game *, const struct move_discard *);
	bool (*skip)(const struct game *);
	bool (*quit)(const struct game *);
} Rule;

typedef struct game
{
	int turn;
	int playerNum;
	struct player player[MAX_PLAYER];
	struct board board;
	struct bag bag;
	struct dict dict;
	struct rule rule;
} Game;

typedef struct word word_t;
typedef struct tile tile_t;
typedef struct loc loc_t;
typedef struct board board_t;
typedef struct coor coor_t;
typedef struct move_place move_place_t;
typedef struct move_discard move_discard_t;
typedef struct tile_adjust tile_adjust_t;
typedef struct tile_adjust tile_adjusts_t[RACK_SIZE];
typedef struct adjust adjust_t;
typedef struct move move_t;
typedef struct bag bag_t; 
typedef struct dir dir_t;
typedef struct path_dot path_dot_t;
typedef struct path_horz path_horz_t;
typedef struct path_vert path_vert_t;
typedef struct path path_t;
typedef struct action_place action_place_t;
typedef struct action_discard action_discard_t;
typedef struct action action_t;
typedef struct ai_share ai_share_t;
typedef struct player player_t;
typedef struct dict dict_t;
typedef struct rule rule_t;
typedef struct game game_t;

void	mk_adjust(const player_t *, adjust_t *);
void	adjust_swap(int, int, tile_adjusts_t);
adjust_err_t	find_adjust_err(const adjust_t *, const player_t *);
void	apply_adjust(const tile_adjusts_t, player_t *);
void	mk_action(const game_t *, const move_t *, action_t *);
bool	apply_action(const action_t *, game_t *);
void	next_turn(game_t *);
cmp_t	cmp_word(const word_t *, const word_t *);
void	move_clear(move_t *);
void	action_clear(action_t *);
void	rm_rack_tile(const int *, const int, player_t *);
void	rack_shift(player_t *);
void	rack_refill(bag_t *, player_t *);
bool	end_game(const game_t *);
int	find_winner(const game_t *);
bool	valid_rack_idx(int);
bool	valid_board_idx(coor_t);
int	rack_count(const player_t *);
int	adjust_tile_count(const tile_adjusts_t);
bool	word_valid(const word_t *, const dict_t *);
bool	board_empty(const board_t *b);
bool	bag_full(const bag_t *);
bool	bag_empty(const bag_t *);
int	bag_count(const bag_t *);
int	bag_size(const bag_t *);
int	tile_score(const tile_t *);
bool	vowel(letter_t);
bool	constant(letter_t);

#endif
