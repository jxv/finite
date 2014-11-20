INC := -I include
INC += -I include/core
INC += -I include/ui
INC += -I include/util

SRC := $(wildcard *.c)
SRC += $(wildcard core/*.c)
SRC += $(wildcard ui/*.c)
SRC += $(wildcard util/*.c)

CFLAGS := -std=c99
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
