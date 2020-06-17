/*
* An instantiation of the game. This structure is initialized
* in the main of the server
* game.h is the header file for game.c
*
* Team JYRE, CS50 Spring 2019
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "client_struct.h"

//game structure
typedef struct game {
	int nuggetsLeft; //number of nuggets left to take
	int pilesLeft;	//actual amount of piles left in the game
	char* masterMap; //1D array with all information about the game
} game_t;

//structure that contains the information
//for what the user runs into when the move
typedef struct goldOrId {
	char id;
	int gold;
} goldOrId_t;


/* The random() and srandom() functions are provided by stdlib,
 * but for some reason not declared by stdlib.h, so we declare here.
 */
long int random(void);
void srandom(unsigned int seed);

/******************************* gameNew *******************************/
/*
 * instantiates a new game module
 *  -filename is the string of the file that will be turned into a map
 *  -rand is the random integer by which all random numbers generated are based on
 */
game_t* gameNew(char* filename, int rand);

/******************************* addPlayer *******************************/
/*
 * adds a player to the game module
 *  -client is the data of the player that will be added
 *  -game is the module that will recieve the added player
 */
bool addClient(client_t* client, game_t* game); 

/******************************* moveLeft *******************************/
/*
 * Handles moveleft funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveLeft(client_t* client, game_t* game);

/******************************* moveRight *******************************/
/*
 * Handles moveRight funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveRight(client_t* client, game_t* game);

/******************************* moveUp *******************************/
/*
 * Handles moveUp funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveUp(client_t* client, game_t* game);

/******************************* moveDown *******************************/
/*
 * Handles moveDown funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveDown(client_t* client, game_t* game);

/******************************* moveUpAndLeft *******************************/
/*
 * Handles moveUpAndLeft funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveUpAndLeft(client_t* client, game_t* game);

/******************************* moveUpAndRight *******************************/
/*
 * Handles moveUpAndRight funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveUpAndRight(client_t* client, game_t* game);

/******************************* moveDownAndRight *******************************/
/*
 * Handles moveDownAndRight funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveDownAndRight(client_t* client, game_t* game);

/******************************* moveDownAndLeft *******************************/
/*
 * Handles moveDownAndLeft funtionality. Will return data that tells the server what
 * happened when the client moved
 *  -client is the data of the player that will move
 *  -game is the module that will recieve the movement
 */
goldOrId_t* moveDownAndLeft(client_t* client, game_t* game);

/******************************* goldValue *******************************/
/*
 * returns the random value of the gold pile the user has run into
 *  -game is the module in which the gold was taken
 */
int goldValue(game_t* game);








