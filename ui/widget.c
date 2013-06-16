#include "widget.h"


bool validRackId(int id)
{
	return id >= 0 && id < RACK_SIZE;
}


bool validGridId(struct coor c)
{
	return c.x >= 0 && c.y >= 0 && c.x < BOARD_X && c.y < BOARD_Y;
}


void mkGridWidgetByDim(struct gridWidget *gw)
{
	int i, y, x;

	NOT(gw);

	gw->index.x = 0;
	gw->index.y = 0;
	gw->button  = memAlloc(sizeof(bool*) * gw->height);
	for (i = 0; i < gw->height; i++) {
		gw->button[i] = memAlloc(sizeof(bool) * gw->width);
	}

	for (y = 0; y < gw->height; y++) {
		for (x = 0; x < gw->width; x++) {
			gw->button[y][x] = false;
			gw->button[y][x] = false;
		}
	}
}


void mkRackWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width  = RACK_SIZE;
	gw->height = 1;
	mkGridWidgetByDim(gw);
}


void mkChoiceWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width  = CHOICE_COUNT;
	gw->height = 1;
	mkGridWidgetByDim(gw);
}


void mkBoardWidget(struct gridWidget *gw)
{
	NOT(gw);
	
	gw->width  = BOARD_X;
	gw->height = BOARD_Y;
	mkGridWidgetByDim(gw);
}


void boardWidgetDraw(struct io *io, struct gridWidget *bw, struct board *b, struct coor pos, struct coor dim)
{
	struct tile *t;
	struct SDL_Surface *ts;
	int y, x;
	NOT(io);
	NOT(bw);
	NOT(b);

	for (y = 0; y < BOARD_Y; y++) {
		for (x = 0; x < BOARD_X; x++) {
			t = &b->tile[y][x];
			if (t->type != TILE_NONE) {
				ts = io->tile[t->type][t->letter];
				surfaceDraw(io->screen, ts, x * dim.x + pos.x, y * dim.y + pos.y);
			}
		}
	}
}


void gridWidgetDraw(SDL_Surface *s, struct gridWidget *gw, struct coor pos, struct coor dim)
{
	int y, x;
	SDL_Rect clip;
	Uint32 red, green, color;
	
	NOT(gw);

	clip.w = dim.x - 1;
	clip.h = dim.y - 1;
	red   = SDL_MapRGB(s->format, 255, 0, 0);
	green = SDL_MapRGB(s->format, 0, 255, 0);
	for (y = 0; y < gw->height; y++) {
		for (x = 0; x < gw->width; x++) {
			color  = gw->button[y][x] ? green : red;
			clip.x = pos.x + dim.x * x;
			clip.y = pos.y + dim.y * y;
			SDL_FillRect(s, &clip, color);
		}
	}
}


