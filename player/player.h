/*
* player.h - header file for player.c
*
* Team: JYRE, CS 50, Spring 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include <ncurses.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "message.h"
#include "file.h"

/*******************checkScreenSize*********/
/*
*
* makes sure that the terminal screen size is big enough
* if it is not, display error message to user telling them to make
* it bigger
*
*/

void checkScreenSize();

/*******************readUserInput***********/
/*
*
* reads in input from stdin
* If the client is a spectator (name is "spectator") then the
* only input it acts on is "q" for quit
*/

bool readUserInput();

/*******************parseMessage***********/
/*
*
* parses messages from server
* is called from message loop in main
*/
bool parseMessage(void *arg, const addr_t from, const char* message);

/******************* displayMap ***********/
/*
* called within parseMessage
* receives map from server and displays it
*
*
*/
void displayMap();


/****************** log ***********/
/*
*
* writes messages to the log
*/
void printLog(char* message);

/*******************updateServer***********/
/*
*
* Sends messages to the server after any user input, usually just keystrokes
*/

void updateServer(addr_t to, char* message);

/*******************leaveGame***********/
/*
*
* frees everything
*
*/
void leaveGame();
