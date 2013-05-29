CC=cc
RM=rm

INC=-I .
INC+=-I core
INC+=-I ui
INC+=-I util

SRC=$(wildcard *.c)
SRC+=$(wildcard core/*.c)
SRC+=$(wildcard ui/*.c)
SRC+=$(wildcard util/*.c)

CFLAG= -ansi
CFLAG+= -Werror
CFLAG+= -Wall
CFLAG+= -pedantic
CFLAG+= -O2

LIB=-lc
LIB+=`sdl-config --libs`
LIB+=-lSDL_image

OUT=-o finite

all:
	$(CC) $(CFLAG) `sdl-config --cflags` $(LIB) $(OUT) $(SRC) $(INC)
clean:
	$(RM) $(OUT)
