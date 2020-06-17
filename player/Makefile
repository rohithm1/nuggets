
# Makefile for the "player" program.
#
# Emma Rafkin - May 2019

L = ../support

CC=gcc
CFLAGS=-Wall -pedantic -std=c11 -ggdb -I$L

LIBS = -lcurses -lm
LLIBS = $L/support.a

.PHONY: all clean

all: player

# executable depends on object files
player: player.o $(LLIBS)
	$(CC) $(CFLAGS) player.o $(LLIBS) $(LIBS) -o player


# object files depend on include files
player.o: player.h $L/message.h $L/file.h


clean:
	rm -f player
	rm -f *~ *.o
	rm -rf *.dSYM
