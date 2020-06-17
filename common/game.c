/*
* game.c module
*
* TEAM JYRE May 2019, CS50 Dartmouth
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "map.h"
#include "game.h"
#include "client_struct.h"

static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

game_t* gameNew(char* filename, int rand);
bool addClient(client_t* client, game_t* game); 
goldOrId_t* moveLeft(client_t* client, game_t* game);
goldOrId_t* moveRight(client_t* client, game_t* game);
goldOrId_t* moveUp(client_t* client, game_t* game);
goldOrId_t* moveDown(client_t* client, game_t* game);
goldOrId_t* moveUpAndLeft(client_t* client, game_t* game);
goldOrId_t* moveUpAndRight(client_t* client, game_t* game);
goldOrId_t* moveDownAndRight(client_t* client, game_t* game);
goldOrId_t* moveDownAndLeft(client_t* client, game_t* game);
int goldValue(game_t* game);

/******************************* gameNew *******************************/
/*
 * Constructs a new game_t struct - fills in the mao with the filename
 * seed is used to randomize the amount of piles
 */		
game_t*
gameNew(char* filename, int seed)
{
	//generates a random number based on srandom (done in server)
	int rand = random();

	//create the new game client
	game_t* game = malloc(sizeof(game_t));
	game->masterMap = mapNew(filename);

	//get the number of rows and columns in the map
	int nrows = mapGetRows();
	int ncols = mapGetColumns();


	//random number of piles between max and min
	int piles = GoldMaxNumPiles - GoldMinNumPiles;
	piles = (int)(((float)(rand % 100)/100.0)*(float)piles) + GoldMinNumPiles;
	
	//set the variables in game
	game->pilesLeft = piles;
	game->nuggetsLeft = GoldTotal;

	int count = 0;
	int arrIndex = 0;
    int emptyIndexes[nrows*ncols];	//this is the array of all the empty indexes

    //initialize all the values to 0
    for (int i = 0; i < nrows*ncols; i++) {
		emptyIndexes[i] = 0;
	}

	//loop through all possible index
    for (int i = 0; i < nrows*ncols; i++) {

    	//if the index is empty in the masterMap
	    if (checkIfEmpty(game->masterMap, i)) {
	    	//set the next index in the array to that index
	    	emptyIndexes[count] = i;
	    	count++;
	    	arrIndex++;
	    }

	    //else keep it at 0
	    else {
	    	emptyIndexes[arrIndex] = 0;
	    	arrIndex++;
	    }
	}

	//determine how many places to skip when placing gold
	int place = (int)((float)(count/piles));

	//clean up the values
	for (int i = 0; i < nrows*ncols; i++) {
		if (emptyIndexes[i] > nrows*ncols || emptyIndexes[i] < 0) {
			emptyIndexes[i] = 0;
		}
	}

	//while there are still piles to be added
	for (int i = 0; piles != 0; ) {

		//start over if youve loopde through them all
		if (i > arrIndex) {
			i = 0;
		}

		//else if there is no index there, continue looping
		else if (emptyIndexes[i] == 0) {
			i++;
		} 

		//else add the gold at that index
		else if (addGold(game->masterMap, emptyIndexes[i])) {
			i += place;
			piles--;

		}
	}
	
	return game;
}

/******************************* addPlayer *******************************/
/*
 * adds the player to the map with random placement and default values
 * client is the player that should be added
 * game is the game (contains the masterMap) that the client is added to
 */		
bool
addClient(client_t* client, game_t* game)
{
	//need to get dimensions for math + random calculations
	int nrows = mapGetRows();
	int ncols = mapGetColumns();

	//default values
	int count = 0;
	int arrIndex = 0;

	int emptyIndexes[nrows*ncols];

	//initialize the array to 0 value
	for (int i = 0; i < nrows*ncols; i++) {
		emptyIndexes[count] = 0;
	}

	//loop through the array and add empty indexes
	for (int i = 0; i < nrows*ncols; i++) {

		//if it is empty in the map, add to the array
		if (checkIfEmpty(game->masterMap, i)) {
			emptyIndexes[count] = i;
			count++;
			arrIndex++;
		}

		//else keep it at 0
		else {
			emptyIndexes[arrIndex] = 0;
			arrIndex++;
		}
	}

	int idx = 0;
	int num = (random() % (count));
	bool empty = false;

	//while an empty index hasent been found
	while (!empty) {
		if (num > count) {
			num = 0;
		}
		else if (emptyIndexes[num] == 0) {
			num++;
		}
		else if (emptyIndexes[num] != 0) {
			idx = emptyIndexes[num];
			empty = true;
		}
	}

	idx++; //turn it into a non-index
	int posX = idx % ncols;
	int posY = (idx - posX)/ncols;
	posY++;

	//basd on random position set the values
	client->position.x = posX;
	client->position.y = posY;
	int client_index = ncols*(posY - 1) + posX - 1;

	//make a. new map
	char* map = malloc(nrows*ncols*10*sizeof(char));
    sprintf(map, "%s", game->masterMap);
    client->map = map;

    //copy an empty version of the masterMap into the clientMap 
    for (int i = 0; i < nrows*ncols; i++) {
	    if (i != client_index && client->map[i] != '\n') {
	      client->map[i] = ' ';
	    }
	}
	return true;
}

/******************************* goldValue *******************************/
/*
 * when a player runs into a gold pile, this function will randomize the value
 * of that gold pile. 
 */		
int
goldValue(game_t* game) 
{
	int value = 0;
	int maxValue = game->nuggetsLeft - game->pilesLeft + 1;


	//if there is only one more pile - all the value is in that
	if (game->pilesLeft == 1) {
		value = game->nuggetsLeft;
		return value;
	}

	//randomize the value of the gold pile
	if (maxValue > 30) {
		value = random() % 30 + 1;
	}
	else {
		value = random() % maxValue + 1;
	}
	return value;
}

/******************************* moveLeft *******************************/
/*
 * Handles moveLeft funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveLeft(client_t* client, game_t* game)
{
	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index - 1;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}


/******************************* moveRight *******************************/
/*
 * Handles moveRight funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveRight(client_t* client, game_t* game)
{
	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index + 1;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}

/******************************* moveUp *******************************/
/*
 * Handles moveUp funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveUp(client_t* client, game_t* game)
{

	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index - ncols;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}

/******************************* moveDown *******************************/
/*
 * Handles moveDown funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveDown(client_t* client, game_t* game)
{

	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index + ncols;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}

/******************************* moveUpAndLeft *******************************/
/*
 * Handles moveUpAndLeft funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveUpAndLeft(client_t* client, game_t* game)
{
	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index - ncols - 1;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}

/******************************* moveUpAndRight *******************************/
/*
 * Handles moveUpAndRight funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveUpAndRight(client_t* client, game_t* game)
{
	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index - ncols + 1;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}

/******************************* moveDownAndRight *******************************/
/*
 * Handles moveDownAndRight funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveDownAndRight(client_t* client, game_t* game)
{
	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index + ncols + 1;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}


/******************************* moveDownAndLeft *******************************/
/*
 * Handles moveDownAndLeftt funtionality. Will return a struct goldOrId to tell the server
 * what the use ran into, if they did run into anything
 */	
goldOrId_t*
moveDownAndLeft(client_t* client, game_t* game)
{
	int currX = client->position.x;
	int currY = client->position.y;

	int ncols = mapGetColumns();

	goldOrId_t* goldId = malloc(sizeof(goldOrId_t));

	//this is the index of the client, not where it needs to move
	int index = ncols*(currY - 1) + currX - 1;

	//get the location of the place right below it
	int moveTo = index + ncols - 1;


	//if the index is valid
	if (index >= 0) {

		///update the map with the new index of wher the client wants to move
		char temp;
		if ((temp = updateMap(game->masterMap, moveTo, client)) != '\0') {

			//if they ran into a gold pile
			if (temp == 'g') {
				//grabbing the random value of the gold
				int value = goldValue(game);
				//update instance variables
				game->pilesLeft--;
				game->nuggetsLeft = game->nuggetsLeft - value;

				//will tell the server something was grabbed
				goldId->gold = value;
				client->score = client->score + goldId->gold;
				goldId->id = '\0';
				return goldId;
			}

			//if they run into an empty space
			else if (temp == 'w') {
				goldId->gold = 0;
				goldId->id = 'w';
				return goldId;
			}

			//if they run into a client
			else if (temp != '\0') {
				goldId->gold = -1;
				goldId->id = temp;
				return goldId; 
			}	
		}
		else {
			goldId->gold = -1;
			goldId->id = '\0';
		}
	}
	return goldId;
}







