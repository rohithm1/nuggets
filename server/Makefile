# Makefile for the "server" program.
#
#JYRE- May 2019

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

