/*
* map.h - header file for map.c
*
* Team: JYRE, CS 50, Spring 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "client_struct.h"
#include "file.h"

/************* mapNew **********/
/*
*
* reads in a .txt file from the server and
* turns it into a 1-dimensional array saved in map
*
*/
char* mapNew(char* filename);


/*********** updateMap ************/
/*
* simply switches out the character at the given position
* makes changes to the client's map
*
* returns 'g' if there was gold there, the id of the other client
* if it replaced the other client and they switched spots, 'w' if
* the position is empty, and NULL otherwise
*
*/
char updateMap(char *map, int new_idx, client_t* client);


/*********** visibility ************/
/*
* calculates what can be seen by the client at a certain location
* adds to the parts of the map that the client knows if necessary
*
* input: client map, client
*/
void visibility(char *map, client_t* client);


/******************** nextToWall *********************/
/*
* check if a certain position is next to a wall
*/
int nextToWall(int visible[], int index, int emptySpot);


/********************* addGold *********************/
/*
* checks if a position can have gold and adds gold if valid
* returns true if gold was added, false otherwise
*
*/
bool addGold(char *map, int idx);


/******************* checkIfClientValid ***********/
/*
* checks if the client can move to a certain position (. or * or #)
* returns true if it is somewhere the client can move, false otherwise
*
*/
bool checkIfClientPosValid(char *map, int idx);


/******************* checkIfGold ****************/
/*
* checks if the gold is at a certain position (*)
* returns true if there is gold there, false otherwise
*
*/
bool checkIfGold(char *map, int idx);


/******************* checkIfClient ****************/
/*
* checks if another client is at a certain position
* returns true if there is a client there, false otherwise
*
*/
bool checkIfClient(char *map, int idx);


/******************* checkIfEmpty *******************/
/*
* checks if a certain position is empty (.)
* returns true if it is empty, false otherwise
*
*/
bool checkIfEmpty(char *map, int idx);


/******************* checkIfWall *******************/
/*
* checks if a certain position is a wall (| or + or -)
* returns true if it is a wall, false otherwise
*
*/
bool checkIfWall(char* map, int idx);


/******************* checkIfMovable1 *******************/
/*
* checks if a certain position is moveable
* returns true if it is possible, false otherwise
* for quadrant 1
*
*/
bool checkIfMovable1(char* map, int move, int north, int east);


/******************* checkIfMovable2 *******************/
/*
* checks if a certain position is moveable
* returns true if it is possible, false otherwise
* for quadrant 2
*
*/
bool checkIfMovable2(char* map, int move, int north, int west);


/******************* checkIfMovable3 *******************/
/*
* checks if a certain position is moveable
* returns true if it is possible, false otherwise
* for quadrant 3
*
*/
bool checkIfMovable3(char* map, int move, int south, int west);

/******************* checkIfMovable4 *******************/
/*
* checks if a certain position is moveable
* returns true if it is possible, false otherwise
* for quadrant 4
*
*/
bool checkIfMovable4(char* map, int move, int south, int east);

/******************* checkIfInPathway *******************/
/*
* checks if a certain position is in a pathway (#)
* returns true if it is, false otherwise
*
*/
bool checkIfInPathway(int index);

/******************* mapGetRows *******************/
/*
* return number of rows in the map
*
*/
int mapGetRows();


/******************* mapGetColumns *******************/
/*
* return number of columns in the map
*
*/
int mapGetColumns();


/****************deleteMap*************/
/*
* frees everything at the end of the game
*
*/
void deleteMap();
