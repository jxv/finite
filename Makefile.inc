INC := -I src
INC += -I src/core
INC += -I src/ui
INC += -I src/util

SRC := $(wildcard src/*.c)
SRC += $(wildcard src/core/*.c)
SRC += $(wildcard src/ui/*.c)
SRC += $(wildcard src/util/*.c)

CFLAGS := -ansi
# CFLAGS +=-Werror
# CFLAGS +=-Wextra
CFLAGS += -Wall
CFLAGS += -pedantic
CFLAGS += -O3
CFLAGS += -DDEBUG
# CFLAGS += $(shell sdl-config --cflags)

LIB := -lc
LIB += -lm
LIB += -lpthread
# LIB += $(shell sdl-config --libs)
LIB += -lSDL_image
LIB += -lSDL_mixer

OBJ := $(SRC:.c=.o)

OUT := finite

RM := rm -f
