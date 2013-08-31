#include <string.h>


#include "core.h"
#include "init.h"
#include "print.h"
#include "term.h"


int getLine(char *line, size_t s)
{
	int c, len;

	NOT(line);

	line = fgets(line, s, stdin);
	line[strcspn(line, "\n")] = '\0';
	len = strlen(line);
	if (len == s - 1) {
		while ((c = getchar()) != '\n' && c != EOF);
	}
	return len;
}


bool termInit(struct Game *g)
{
	NOT(g);

	g->turn = 0;
	g->playerNum = 2;
	if (!dictInit(&g->dict, RES_PATH "dict.txt")) {
		return false;
	}
	boardInit(&g->board);
	bagInit(&g->bag);
	playerInit(&g->player[0], &g->bag, g->rackSize);
	playerInit(&g->player[1], &g->bag, g->rackSize);
	return true;
}


int fdNextChar(const char *str, int len, const char c)
{
	int i;

	NOT(str);

	for (i = 0; i < len; i++) {
		if (str[i] == c) {
			return i;
		}
	}
	return -1;
}


bool parseToPlace(struct MovePlace *mp, const char *str, int len)
{
	int x, y, r;
	int i, j, k;
	size_t  n;

	NOT(mp), NOT(str);
	
	n = 0;
	i = 0;
	k = 0;
	for (;;) {
		if (k == RACK_SIZE) {
			break;
		}
		j = fdNextChar(str + i, len - i, '(');
		if (j == -1) {
			break;
		} else {
			i += j;
		}
		n = sscanf(str + i, "(%d,%d,%d)", &x, &y, &r);
		if (n == 3) {
			mp->rackIdx[k] = r;
			mp->coor[k].x  = x;
			mp->coor[k].y  = y;
			k++;
		} else {
			break;
		}
		i++;
	}
	mp->num = k;
	return n == 3 && k <= RACK_SIZE;	/* n == 3 && k <= RACK_SIZE, on success */
}


bool parseToDiscard(struct MoveDiscard *md, const char *str, int len)
{
	int r, k, i, j;
	size_t n;
	
	NOT(md), NOT(str);

	i = 0;
	n = 0;
	k = 0;
	for (;;) {
		if (k == RACK_SIZE) {
			break;
		}
		j = fdNextChar(str + i, len - i, '(');
		if (j == -1) {
			break;
		} else {
			i += j;
		}
		n = sscanf(str + i, "(%d)", &r);
		if (n == 1) {
			md->rackIdx[k] = r;
			k++;
		} else {
			break;
		}
		i++;
	}
	md->num = k;
	return n == 1 && k <= RACK_SIZE;
}


void termGetMoveType(struct Move *m)
{
	char line[256];

	NOT(m);

	moveClr(m);
	printf("\n0: MOVE_PLACE\n1: moveSkip\n2: moveDiscard\n3: moveQuit\n");
	do {
		int i;
		m->type = moveInvalid;
		getLine(line, sizeof(line));
		if (sscanf(line, "%d", &i) == 1) {
			switch (i) {
			case  0: m->type = movePlace; break;
			case  1: m->type = moveSkip; break;
			case  2: m->type = moveDiscard; break;
			case  3: m->type = moveQuit; break;
			default: m->type = moveInvalid; break;
			}
		}
	} while (m->type == moveInvalid);
}


void termMove(struct Move *m)
{
	char line[256];

	NOT(m);

	switch (m->type) {
	case movePlace: {
		printf("Enter the rack index of tiles to place (x,y,rack-index):\n");
		getLine(line, sizeof(line));
		if (!parseToPlace(&m->data.place, line, strlen(line))) {
			printf("[err: bad input format]\n");
			m->type = moveInvalid;
		} 
		break;
	}
	case moveSkip: {
		break;
	}
	case moveDiscard: {
		printf("Enter the rack index of tiles to place (rack-index) ... (rack-index):\n");
		getLine(line, sizeof(line));
		if (!parseToDiscard(&m->data.discard, line, strlen(line))) {
			printf("[err: bad input format]\n");
			m->type = moveInvalid;
		}
		break;
	}
	case moveQuit: {
		break;
	}
	default: break;
	}
}


int term()
{
	struct Game g;
	struct Move m;
	struct Action a;
	int winnerIdx;

	if (!termInit(&g)) {
		return EXIT_FAILURE;
	}
	puts("=======");
	puts("FINITE");
	puts("=======");
	do {
		printScore(&g);
		puts("===============");
		printf("Turn: PLAYER_%d\n", g.turn);
		puts("===============");
		printBoard(&g.board);
		printRack(&g.player[g.turn]);
		do {
			do {
				termGetMoveType(&m);
				termMove(&m);
				m.playerIdx = g.turn;
			} while (m.type == moveInvalid);
			actionClr(&a);
			mkAction(&a, &g, &m);
			if (a.type == actionInvalid) {
				printActionErr(a.data.err);
			}
		} while(a.type == actionInvalid);
		if (applyAction(&g, &a)) {
			nextTurn(&g);
		}
	} while (!endGame(&g));
	winnerIdx = fdWinner(&g);
	if (winnerIdx != -1) {
		printf("\nPLAYER_%d WON!\n", winnerIdx);
	} else {
		printf("\nTIE!\n");
	}
	dictQuit(&g.dict);
	return EXIT_SUCCESS;
}


