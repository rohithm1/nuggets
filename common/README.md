# common library

This library contains two modules useful for our CS50 final project shared amongst the other module

Team members: Rohith Mandavilli and Joanna Liu

## 'game' module

This module provides a way to update the `game` struct 
allows the user to addplayers, moveplayers, etc.

See `game.h` for function details.


## 'map' module

Provides a module to standardize updating maps across all the modules.
See `map.h` for function.

## compiling

To compile,

	make 

To clean,

	make clean

## using

In a typical use, assume this library is a subdirectory named `common`, within a directory where some main program is located.
The Makefile of that main directory might then include content like this:

```make
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
```

This approach allows the main program to be built (or cleaned) while automatically building (cleaning) the library as needed.

## testing

testing was primarily completed through running the server.c file since there were added in functions to call game.c modules. 

To clarify, if you wanted to test this file, you would need to make the entire project and run player and server.

1. run `make` in the entire directory, that Makefile should compile all the subdirectories
2. run `./server [textfile] [seed]` in the server subdirectory
3. run `./player [hostname] [portname] [name]` in the client subdirectory






