#ifndef SCABS_CORE_H
#define SCABS_CORE_H

#include <stdio.h>
#include <stdlib.h>

#include "dbg.h"
#include "mem.h"

#define RES_PATH		"res/"

#define BOARD_X			15
#define BOARD_Y			15
#define BOARD_SIZE		BOARD_Y	/* Max among BOARD_X and BOARD_Y */
#define RACK_SIZE		7
#define BAG_SIZE		(100+1)	/* needs extra element for size check */
#define MAX_PLAYER		4

#define VALID_BOARD_X(x)	RANGE(x,0,BOARD_X-1)
#define VALID_BOARD_Y(y)	RANGE(y,0,BOARD_Y-1)
#define VALID_BOARD_SIZE(s)	RANGE(s,0,BOARD_SIZE-1)


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
} tile_t;


typedef enum
{
	SQ_NORMAL = 0,
	SQ_DBL_LET,
	SQ_DBL_WRD,
	SQ_TRP_LET,
	SQ_TRP_WRD,
	SQ_FREE,
	SQ_COUNT
} sq_t;


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
} letter_t;


typedef enum
{
	MOVE_INVALID = -1,
	MOVE_PLACE = 0,
/*	MOVE_SWAP, */
	MOVE_DISCARD,
	MOVE_SKIP,
	MOVE_QUIT,
	MOVE_COUNT
} move_t;


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
} action_err_t;


typedef enum
{
	DIR_INVALID = -1,
	DIR_RIGHT = 0,
	DIR_DOWN,
	DIR_COUNT
} dir_t;


typedef enum
{
	PATH_INVALID = -1,
	PATH_DOT = 0,
	PATH_HORZ,
	PATH_VERT,
	PATH_COUNT
} path_t;


typedef enum
{
	CMP_LESS = -1,
	CMP_EQUAL = 0,
	CMP_GREATER = 1
} cmp_t;


typedef enum
{
	ACTION_INVALID = -1,
	ACTION_PLACE = 0,
/*	ACTION_SWAP, */
	ACTION_DISCARD,
	ACTION_SKIP,
	ACTION_QUIT,
	ACTION_COUNT
} action_t;


struct word
{
	int 		len;
	letter_t	letter[BOARD_SIZE];
};


struct tile
{
	tile_t		type;
	letter_t	letter;
};


struct board
{
	struct tile	tile[BOARD_Y][BOARD_X];
	sq_t		sq[BOARD_Y][BOARD_X];
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


struct place
{
	int		num;
	int		rack_id[RACK_SIZE];
	struct coor	coor[RACK_SIZE];
};


struct discard
{
	int		num;
	int		rack_id[RACK_SIZE];
};


struct move
{
	move_t		type;
	int		player_id;
	union {
		struct place			place;
		struct discard			discard;
	} data;
};


struct bag
{
	int head;
	int tail;
	struct tile	tile[BAG_SIZE];
};


struct dir
{
	dir_t		type;
	int		x;
	int		y;
	int		length;
	int		pos[BOARD_SIZE];
};


struct path
{
	path_t type;
	union {
		struct {
			struct dir		right;
			struct dir		down; 
		} dot;
		struct {
			struct dir		right;
			struct dir		down[BOARD_X];
		} horz;
		struct {
			struct dir		right[BOARD_Y];
			struct dir		down;
		} vert;
	} data;
	struct board board;
};


struct action
{
	action_t type;
	int player_id;
	union {
		struct {
			int			score;
			int			num;
			int			rack_id[RACK_SIZE];
			struct path		path;
		} place;
		struct discard discard;
		action_err_t	err;
	} data;
};


struct dict
{
	long		num;
	letter_t 	**word;
	int		*len;
	struct word	*words;
};


struct game
{
	int			turn;
	int			player_num;
	struct player		player[MAX_PLAYER];
	struct board		board;
	struct bag		bag;
	struct dict	dict;
};


void mk_action(struct action*, struct game*, struct move*);
bool apply_action(struct game*, struct action*);
void next_turn(struct game*);
int cmp_word(letter_t*, int, letter_t*, int);
void clr_move(struct move*);
void clr_action(struct action*);
void remove_from_rack(struct player*, int*, int);
void shift_rack(struct player*);
void refill_rack(struct player*, struct bag*);
bool end_game(struct game *g);
int fd_winner(struct game *g);

#endif


