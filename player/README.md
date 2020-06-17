# Player

This diretory contains the player module for the CS50 nuggets game. The main functionality allows players to connect to servers and play the game. This contains the functionality for both the player and the spectator. 

TEAM members: Emma Rafkin

## compiling

To compile,

	make
  or make player

To clean,

	make clean

## using
The Makefile of the player directory might then include content like this:

```make
L = ../support
C = ../common


PROG = server
OBJS = server.o 
LLIBS = $L/support.a $C/common.a

CFLAGS=-Wall -pedantic -std=c11 -ggdb -I$L -I$C -I$G -I$M
CC = gcc

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

server.o: server.h $C/client_struct.h $L/support.a $C/common.a


.PHONY: all clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(PROG)
```



##testing

The player requires a `server` to be running *before* player is called.

The user's terminal window must be large enough to play the game, however this module accounts for that and asks the user to resize their screen if necessary. 

The player logs out useful messages to stderr, and since it uses the ncurses library (and thus the screen is occupied), it is necessary that stderr is redirected.

An example of how player can be run/tested is with the following commandline:

`./player 2> player.log hostname portname [playername]`

To spectate the nuggets game rather than play, run the same line but without a playername (which is why it is in brackets above to denote optionality):
`./player 2> player.log hostname portname`

The player module can be used with valgrind to detect any memory errors. Note that since this module uses ncurses, there are will always be memory leaks in the "still remaining" section because there is memory allocated for the screen upon quitting from the program.
