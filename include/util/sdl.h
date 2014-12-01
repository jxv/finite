#ifndef	FINITE_SDL_UTIL_H
#define	FINITE_SDL_UTIL_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_net.h>

#define ALPHA_R		0xff
#define ALPHA_G		0x00
#define ALPHA_B		0xff
#define SCREEN_BPP	32

enum key_state_type
{
	KEY_STATE_UNTOUCHED = 0,
	KEY_STATE_PRESSED,
	KEY_STATE_HELD,
	KEY_STATE_RELEASED,
	KEY_STATE_COUNT,
};

enum axis_state_type
{
	AXIS_STATE_IN_DEAD_ZONE = 0,
	AXIS_STATE_EXIT_DEAD_ZONE,
	AXIS_STATE_OUT_DEAD_ZONE,
	AXIS_STATE_ENTER_DEAD_ZONE,
	AXIS_STATE_COUNT,
};

typedef enum key_state_type key_state_type_t;
typedef enum axis_state_type axis_state_type_t;

struct key_state
{
	key_state_type_t type;
	float time;
};

struct axis_state
{
	axis_state_type_t type;
	float dead_zone;
	float value;
	float time;
};

typedef struct key_state key_state_t;
typedef struct axis_state axis_state_t;

void surface_free(SDL_Surface *s);
SDL_Surface *surface_load(const char *filename);
SDL_Surface *surface_alpha_load(const char *filename);
void delay(int st, int et, int fps);
void surface_draw(SDL_Surface *s0, SDL_Surface *s1, int x, int y);
SDL_Surface *surface_cpy(SDL_Surface *s);
void axis_state_update(axis_state_t *as);

#endif
