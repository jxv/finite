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

CFLAG=-ansi
#CFLAG+=-Werror
CFLAG+=-Wall
CFLAG+=-pedantic
CFLAG+=-O3
CFLAG+=-DDEBUG
CFLAG+=`sdl-config --cflags`

LIB=-lc
LIB=-lpthread
LIB+=`sdl-config --libs`
LIB+=-lSDL_image

OUT=finite

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAG) $(INC) $(LIB)  
clean:
	$(RM) $(OUT)
