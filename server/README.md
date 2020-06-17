#Server

This directory contains the main server code functionality that parses messages, sends messages, and uses game.c and map.c module functions to edit the `game` structure.

TEAM members: Rohith Mandavilli & Yash Kukadia

##compiling

To compile,
`make`

To clean,

`make clean`

##using 

The Makefile of the server directory might then include content like this:

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

*note: if you want to change the max amount of players, you must go into the code, change the definition of size and the actual constant for the amount of players to declare the global variable correctly*

##testing

The server requires a player to be run and connect to the server.

An example of how server can be run/tested is with the following commandline:

`./server [map textfile] [seed] 2>server.log`

The server logs out useful messages to stderr, it is necessary that stderr is redirected.

After running the server, a player must connect and start playing

An example of how player can be run/tested is with the following commandline:
`./player 2> player.log [hostname] [portname] [playername]`


The server module can be used with valgrind to detect any memory errors.


