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

CFLAGS:=-ansi
#CFLAGS+=-Werror
CFLAGS+=-Wall
CFLAGS+=-pedantic
CFLAGS+=-O3
CFLAGS+=-DDEBUG
CFLAGS+=$(shell sdl-config --cflags)

LIB:=-lc
LIB+=-lm
LIB+=-ldosk
LIB+=-lpthread
LIB+=$(shell sdl-config --libs)
LIB+=-lSDL_image
LIB+=-lSDL_mixer
LIB+=-lSDL_net

OBJ:=$(SRC:.c=.o)

OUT=finite

%.o:	%.c
	$(CC) $(CFLAGS) $(INC) $(LIB) -o $@ -c $<

all:	$(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(CFLAG) $(INC) $(LIB)  
clean:
	$(RM) $(OUT) $(OBJ)
