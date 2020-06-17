/*
* map.c - the map module
*
* Team: JYRE, CS 50, Spring 2019
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "map.h"
#include "file.h"
#include "client_struct.h"



// global variable
static int columns;
static int rows;
static char* original_map;


bool checkIfEmpty(char *map, int idx);
bool checkIfGold(char *map, int idx);
bool checkIfClient(char *map, int idx);
bool checkIfTunnel(char *map, int idx);
bool checkIfWall(char* map, int idx);
bool checkIfMovable1(char* map, int move, int north, int east);
bool checkIfMovable2(char* map, int move, int north, int west);
bool checkIfMovable3(char* map, int move, int south, int west);
bool checkIfMovable4(char* map, int move, int south, int east);
bool checkIfInPathway(int index);
int nextToWall(int array[], int index, int emptySpot);



/************* mapNew **********/
/* see map.h for more information */
char*
mapNew(char* filename)
{
    // make the original map
    FILE *fp;

    // test if the file is openable
    fp = fopen(filename, "r");

    char *line;
    line = freadlinep(fp);
    // find the length of each line - includes the new line character
    columns = strlen(line) + 1;
    // count number of lines
    rows = lines_in_file(fp);
    free(line);
    fclose(fp);

    // allocate memory for each line
    char map[(rows * columns * 10) + 1];
    // add each character into a 1D array
    fp = fopen(filename, "r");
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        // add the character to a map
        map[i] = c;
        i++;
    }
    map[i] = '\0';
    fclose(fp);

    // make another map to be set to the original
    char temp[(rows * columns * 10) + 1];
    for (int i = 0; i<rows*columns; i++) {
        temp[i] = map[i];
    }
    original_map = temp;
    // make a map pointer to return
    char* mapPointer = malloc (((rows*columns*10) +1) * sizeof(char));
    strcpy(mapPointer, map);
    return mapPointer;
}


/*********** updateMap ************/
/* see map.h for more information */
char
updateMap(char *map, int new_idx, client_t* client)
{

    // if the client was not on the map before
    if (client->position.x == -1) {
        map[new_idx] = client->id;
        return '\0';
    }

    // original position of the client (in index format)
    int pos = ((client->position.y - 1) * columns) + client->position.x -1;
    // invalid index
    if (new_idx < 0 || strlen(map) < new_idx) {
        return '\0';
    }

    // if the new position is empty, just replace with the client id
    // replace the position that the client was at with what was on the original map
    if (checkIfEmpty(map, new_idx) || checkIfTunnel(map, new_idx)) {
        map[new_idx] = client->id;
        map[pos] = original_map[pos];
        return 'w';
    }

    // if the new position has gold, still replace with the client id
    // replace the position that the client was at with what was on the original map
    // remove the gold from the original map
    if (checkIfGold(map, new_idx)) {
        map[new_idx] = client->id;
        map[pos] = original_map[pos];
        return 'g';
    }

    // if the new position has a client, still replace with the client id
    // replace the position that the client was at with the other client's id
    // the game needs to remember to change client->pos
    if (checkIfClient(map, new_idx)) {
        printf("%s\n", map);
        map[pos] = map[new_idx];
        map[new_idx] = client->id;
        // if return the other client id, remember to update the other client's position
        return map[pos];
    }

  return '\0';
}


/******************** visibility *********************/
/* see map.h for more information */
void
visibility(char *map, client_t* client)
{
    int client_index = ((client->position.y - 1) * columns) + client->position.x -1;

    // make everything . for now
    for (int i = 0; i < rows*columns; i++) {
        if (client->map[i] == '*' || checkIfClient(client->map, i)) {
            client->map[i] = '.';
        }
    }

    // up count
    int up = 1;
    bool hitWallUp = false;
    // keep incrementing up until you hit a wall
    while (!hitWallUp) {
        int toCheck = client_index - up*columns;
        if (!checkIfWall(map, toCheck)) {
            up++;
        }
        else {
            hitWallUp = true;
        }
    }

    // down count
    int down = 1;
    bool hitWallDown = false;
    // keep incrementing down until you hit a wall
    while (!hitWallDown) {
        int toCheck = client_index + down*columns;
        if (!checkIfWall(map, toCheck)) {
            down++;
        }
        else {
            hitWallDown = true;
        }
    }

    // right count
    int right = 1;
    bool hitWallRight = false;
    // keep incrememnting right until you hit a wall
    while (!hitWallRight) {
        int toCheck = client_index + right;
        if (!checkIfWall(map, toCheck)) {
            right++;
        }
        else {
            hitWallRight = true;
        }
    }

    // left count
    int left = 1;
    bool hitWallLeft = false;
    // keep incrememnting left until you hit a wall
    while (!hitWallLeft) {
        int toCheck = client_index - left;
        if (!checkIfWall(map, toCheck)) {
            left++;
        }
        else {
            hitWallLeft = true;
        }
    }

    // make an array of visible indexes
    int t = 0;
    int visible[rows*columns*10];
    visible[t] = client_index;
    t++;

    // set everything in the array to 0
    for (int i = 1; i < rows*columns; i++) {
        visible[i] = 0;
    }

    // set everything above to visible until it hits a wall
    for (int i = 1; i < up; i++) {
        visible[t] = client_index - columns*i;
        t++;
    }

    // set everything below to visible until it hits a wall
    for (int i = 1; i < down; i++) {
        visible[t] = client_index + columns*i;
        t++;
    }

    // set everything right to visible until it hits a wall
    for (int i = 1; i < right; i++) {
        visible[t] = client_index + i;
        t++;
    }

    // set everything left to visible until it hits a wall
    for (int i = 1; i < left; i++) {
        visible[t] = client_index - i;
        t++;
    }

    // set everything in quadrant 1 (NE) to visible
    for (int north = 1; north < up; north++) {
        for (int east = 1; east < right; east++) {
            int move = client_index;
            bool continueSlope = true;
            while (continueSlope) {
                // set visibility by each possible slope in the quadrant
                if (!checkIfWall(map, move) && !checkIfInPathway(client_index)) {
                    continueSlope = checkIfMovable1(map, move, north, east);
                    if (continueSlope) {
                        move = move + east - north*columns;
                        if (move != client_index && !checkIfWall(map, move)) {
                            visible[t] = move;
                            t++;
                        }
                    }
                }
                else {
                    continueSlope = false;
                }
            }
        }
    }

    // set everything in quadrant 2 (NW) to visible
    for (int north = 1; north < up; north++) {
        for (int west = 1; west < left; west++) {
            int move = client_index;
            bool continueSlope = true;
            while (continueSlope) {
                // set visibility by each possible slope in the quadrant
                if (!checkIfWall(map, move) && !checkIfInPathway(client_index)) {
                    continueSlope = checkIfMovable2(map, move, north, west);
                    if (continueSlope) {
                        move = move - west - north*columns;
                        if (move != client_index && !checkIfWall(map, move)) {
                            visible[t] = move;
                            t++;
                        }
                    }
                }
                else {
                    continueSlope = false;
                }
            }
        }
    }

    // set everything in quadrant 3 (SW) to visible
    for (int south = 1; south < down; south++) {
        for (int west = 1; west < left; west++) {
            int move = client_index;
            bool continueSlope = true;
            while (continueSlope) {
                // set visibility by each possible slope in the quadrant
                if (!checkIfWall(map, move) && !checkIfInPathway(client_index)) {
                    continueSlope = checkIfMovable3(map, move, south, west);
                    if (continueSlope) {
                        move = move - west + south*columns;
                        if (move != client_index && !checkIfWall(map, move)) {
                            visible[t] = move;
                            t++;
                        }
                    }
                }
                else {
                    continueSlope = false;
                }
            }
        }
    }

    // set everything in quadrant 4 (SE) to visible
    for (int south = 1; south < down; south++) {
        for (int east = 1; east < right; east++) {
            int move = client_index;
            bool continueSlope = true;
            while (continueSlope) {
                // set visibility by each possible slope in the quadrant
                if (!checkIfWall(map, move) && !checkIfInPathway(client_index)) {
                    continueSlope = checkIfMovable4(map, move, south, east);
                    if (continueSlope) {
                        move = move + east + south*columns;
                        if (move != client_index && !checkIfWall(map, move)) {
                            visible[t] = move;
                            t++;
                        }
                    }
                }
                else {
                    continueSlope = false;
                }
            }
        }
    }

    //just to confirm it is at the last possible index
    for ( ; visible[t] != 0; t++) {
        ;
    }

    int emptySpot = t;

    // deals with walls
    for (int i = 0; i < t; i++) {
        if (visible[i] != 0) {
            emptySpot = emptySpot + nextToWall(visible, i, emptySpot);
        }
    }

    for (int reset = emptySpot + 1; reset < rows*columns*10; reset++) {
        visible[reset] = 0;
    }

    for (int i = 0; i < rows*columns*10; i++) {
        if (visible[i] != 0) {
            // for tunnels
            if (map[visible[i]] == '#') {
                int colNum = visible[i] % columns;
                int rowNum = (visible[i] - colNum)/columns + 1;
                if (client->position.x == colNum + 1 || client->position.y == rowNum) {
                    client->map[visible[i]] = map[visible[i]];
                }
            }
            else {
                client->map[visible[i]] = map[visible[i]];
            }
        }
    }
    // make client the @ symbol
    client->map[client_index] = '@';
}

/******************** nextToWall *********************/
/* see map.h for more information */
int
nextToWall(int visible[], int index, int emptySpot)
{
    int next = 0;
    // right
    if (checkIfWall(original_map, visible[index] + 1)) {
        visible[emptySpot] = visible[index] + 1;
        emptySpot++;
        next++;
    }

    // left
    if (checkIfWall(original_map, visible[index] - 1)) {
        visible[emptySpot] = visible[index] - 1;
        emptySpot++;
        next++;
    }

    // up
    if (checkIfWall(original_map, visible[index] + columns)) {
        visible[emptySpot] = visible[index] + columns;
        emptySpot++;
        next++;
    }

    // down
    if (checkIfWall(original_map, visible[index] - columns)) {
      visible[emptySpot] = visible[index] - columns;
      emptySpot++;
      next++;
    }

    // upper right
    if (checkIfWall(original_map, visible[index] + 1 - columns)) {
        visible[emptySpot] = visible[index] + 1 - columns;
        emptySpot++;
        next++;
    }

    // lower right
    if (checkIfWall(original_map, visible[index] + 1 + columns)) {
        visible[emptySpot] = visible[index] + 1 + columns;
        emptySpot++;
        next++;
    }

    // upper left
    if (checkIfWall(original_map, visible[index] - 1 - columns)) {
        visible[emptySpot] = visible[index] - 1 - columns;
        emptySpot++;
        next++;
    }

    // lower left
    if (checkIfWall(original_map, visible[index] - 1 + columns)) {
        visible[emptySpot] = visible[index] - 1 + columns;
        emptySpot++;
        next++;
    }

    return next;
}


/********************* addGold *********************/
/* see map.h for more information */

bool
addGold(char *map, int idx)
{
    if (checkIfEmpty(map, idx)) {
        // add the gold on the map
        map[idx] = '*';
        return true;
    }
    return false;
}


/******************* checkIfClientValid ***********/
/* see map.h for more information */
bool
checkIfClientPosValid(char *map, int idx)
{
    return checkIfEmpty(map, idx) || checkIfGold(map, idx) || checkIfTunnel(map, idx) || checkIfClient(map, idx);
}

/******************* checkIfGold ****************/
/* see map.h for more information */
bool
checkIfGold(char *map, int idx)
{
    return map[idx] == '*';
}

/******************* checkIfClient ****************/
/* see map.h for more information */
bool
checkIfClient(char *map, int idx)
{
    return isalpha(map[idx]);
}

/******************* checkIfEmpty *******************/
/* see map.h for more information */

bool
checkIfEmpty(char *map, int idx)
{
    if(map[idx] == '.') {
        return true;
    }
    return false;
}

/******************* checkIfTunnel *******************/
/* see map.h for more information */
bool
checkIfTunnel(char *map, int idx)
{
    return map[idx] == '#';
}

/******************* checkIfWall *******************/
/* see map.h for more information */
bool
checkIfWall(char* map, int idx)
{
    if (idx < rows*columns - 1 && idx > 0) {
        if (map[idx] == '|' || map[idx] == '+' || map[idx] == '-' ||  map[idx] == ' ') {
            return true;
        }
    }
    else {
        return true;
    }
    return false;
}

/******************* checkIfMovable1 *******************/
/* see map.h for more information */
bool
checkIfMovable1(char* map, int move, int north, int east)
{
    bool curr = true;
    // checks if you can move somewhere (NE)
    for (int i = 0; i < north; i ++) {
        for (int j = 0; j < east; j++) {
            if (curr) {
                curr = !checkIfWall(map, move + j - i*columns);
            }
        }
    }
    return curr;
}


/******************* checkIfMovable2 *******************/
/* see map.h for more information */
bool
checkIfMovable2(char* map, int move, int north, int west)
{
    bool curr = true;
    // checks if you can move somewhere (NW)
    for (int i = 0; i < north; i ++) {
        for (int j = 0; j < west; j++) {
            if (curr) {
                curr = !checkIfWall(map, move - j - i*columns);
            }
        }
    }

    return curr;
}


/******************* checkIfMovable3 *******************/
/* see map.h for more information */
bool
checkIfMovable3(char* map, int move, int south, int west)
{
    bool curr = true;
    // checks if you can move somewhere (SW)
    for (int i = 0; i < south; i ++) {
        for (int j = 0; j < west; j++) {
            if (curr) {
                curr = !checkIfWall(map, move - j + i*columns);
            }
        }
    }

    return curr;
}

/******************* checkIfMovable4 *******************/
/* see map.h for more information */
bool
checkIfMovable4(char* map, int move, int south, int east)
{
    bool curr = true;
    // checks if you can move somewhere (SE)
    for (int i = 0; i < south; i ++) {
        for (int j = 0; j < east; j++) {
            if (curr) {
                curr = !checkIfWall(map, move + j + i*columns);
            }
        }
    }

    return curr;
}

/******************* checkIfInPathway *******************/
/* see map.h for more information */
bool
checkIfInPathway(int index)
{
    if (original_map[index] == '#') {
        return true;
    }
    return false;
}

/******************* mapGetRows *******************/
/* see map.h for more information */
int
mapGetRows() {
    return rows;
}

/******************* mapGetColumns *******************/
/* see map.h for more information */
int
mapGetColumns() {
    return columns;
}

/****************deleteMap*************/
/* see map.h for more information */
void
deleteMap() {
    free(original_map);
}
