# Makefile for game and map in common
#
# Rohith Mandavilli, April 2019, CS50

L = ../support

OBJS = game.o map.o
LIBS = common.a
LLIBS = $L/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
CC = gcc

$(LIBS): $(OBJS) $(LLIBS)
	ar cr $(LIBS) $(OBJS)

game.o: game.h 
map.o: map.h 

.PHONY: all clean

all: $(LIBS)


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(LIBS) $(LLIBS)