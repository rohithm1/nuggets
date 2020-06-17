/*
* server.h - header file for server.c
*
* Team: JYRE, CS 50, Spring 2019
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include "log.h"
#include "message.h"
#include <time.h>

/*
*
* parses command line arguments and ensures the following format:
* ./server map.txt [seed]
* exits non-zero if incorrect arguments are provided and prints an error message
*
*/
void validateArguments(int noOfArgs, char *arguments[]);



/*
* method called in main within message_loop()
* parses messages from players and spectator
* based on the message, the server will call different
* functions in Server or Grid
*
*/
bool parseMessage (void *arg, const addr_t from, const char *message);

/*
*
* Sends messages to every client. Called by parseMessage
* Probably used within a loop most the of the time, except when
* a client joins or leaves game.


void updateClient(addr_t to, char* message);
*/



/*
*
* free() the memory allocated for the name
* assign id to '!'
*
*/
void removePlayer(int index);


/*
* Checks the id of the last element of client_array
* if it is not 'o', return true
* otherwise, return false
*
*/
bool isSpectatorAvailable();

/*
* Assigns address of the last element of client_array to
* the address passed as an argument
* Assigns the id of the last element of client_array to 'o'
*
*/
void addSpectator(const addr_t from);

/*
* Assigns the id of client_array[26] to 'u' for unoccupied
*/
void removeSpectator();

/*
* Sends a "QUIT" message to the current spectator
* Assigns a new address to client_array[26]
* Assigns the id of client_array[26] to 'o' for occupied
*
*/
void replaceSpectator(const addr_t newSpectator);

/*
* Allocates memory for name of the new player and initializes the name field
* Initializes address to the from address passed as argument
* Initializes active to true
* Initializes score to 0
* returns true if successful, false if there are problems allocating memory for name
*/
bool addPlayer(const char *username, const addr_t from, int index);

/*
* Send game over messages to all clients
* Frees all data using game module
*

void gameEnd();
*/

/*
* iterates through the client_array[]
* returns the index of the first empty spot available
* if no empty spot is available, returns negative number
*/
int checkMaxplayers(client_t *clientArray[]);


void removeAllPlayers();

/*
*
* sends GRID message to addr_t 'from'
*/
void handleGridMessage(const addr_t from);

/*
* sends GOLD message to addr_t after taking n, p and r as arguments
*/
void handleGoldMessage(const addr_t from, int n, int p, int r);

/*
*
* sends GOLD message to all addresses by calling handleGoldMessage() in loop
* treats spectator differently as n=0, p=0 always for spectator
* also treats current player differently as 'n' for everyone else will be 0
*
*/
void handleGoldMessageAll(int index, int n, int p, int r);


/*
* sends QUIT message to addr_t 'from'
*/
void handleQuitMessage(const addr_t from);

/*
* sends GAMEOVER message to addr_t 'from'
*/
void handleGameoverMessage();

/*
* returns the entry of client_array[] that matches with the input id character
* returns NULL if no matches found
*/
client_t* findClient(char inputId);
