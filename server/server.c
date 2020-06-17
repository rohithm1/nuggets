/*
 *  server.c for Nuggets
 *  CS50 Group JYRE
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
#include "client_struct.h"
#include "game.h"
#include "map.h"
#include "server.h"
#include <time.h>

static const int MaxBytes = 65507;          // max number of bytes in a message
static const int MaxPlayers = 26;            // maximum number of players
#define size 27                              //need to define a variable as an integer to declare the global array
client_t *clientArray[size];                // global array of clients

void removePlayer(int index);
void removeAllPlayers();
bool addPlayer(const char *username, const addr_t from, int index);
void addSpectator(const addr_t from);
void removeSpectator();
bool isSpectatorAvailable();
void replaceSpectator(const addr_t new_spectator);
bool parseMessage (void *arg, const addr_t from, const char *message);
void handleGridMessage(const addr_t from);
void handleGoldMessage(const addr_t from, int n, int p, int r);
void handleGoldMessageAll(int index, int n, int p, int r);
void handleQuitMessage(const addr_t from);
void handleGameoverMessage();
client_t* findClient(char inputId);
void validateArguments(int argc, char *arguments[]);
int checkMaxPlayers(client_t *clientArray[]);
void updatePlayers(game_t* game);


int main(int argc, char *argv[])
{
    //default value of the seed is basd on the team
    int seed = time(NULL);

    //need to validate the argument
    validateArguments(argc, argv);
    message_init(stderr);

    //if there is an inputted seed
    if (argc == 3) {
        seed = atoi(argv[2]);
    }

    //randomize the seed
    srandom((unsigned)seed);

    //initialize the game struct
    game_t* game = gameNew(argv[1], seed);
    
    // allocate memory for elements of clientArray[]
    // initialize id values and active flags
    for (int i = 0; i < MaxPlayers + 1; i++) {
        clientArray[i] = malloc(sizeof(client_t));
        clientArray[i]->name = NULL;
        clientArray[i]->active = false;
        clientArray[i]->id = i+65;
        addClient(clientArray[i],game);
    }

    //send the individualized clien to everyone
    updatePlayers(game);

    message_loop(game, 0, NULL, NULL, parseMessage);


    //loop through and free all the data
    for (int i = 0; i < MaxPlayers + 1; i++) {
        free(clientArray[i]);
    }
    message_done();

    //log_done();

}
/******************************* validateArguments *******************************/
/*
 * reads all the input and makes sure its valid
 */     
void validateArguments(int argc, char *arguments[])
{
    FILE *fp;
    // check number of arguments
	if (!(argc == 2) && !(argc == 3)) {
		fprintf(stderr, "usage: ./server map.txt [seed]\n");
		exit(1);
	}

    // check if map.txt exists and is readable
	fp = fopen(arguments[1], "r");
	if (fp == NULL) {
		fprintf(stderr, "map.txt does not exist or is not readable.\n");
		exit(2);
	}
	fclose(fp);

    // check for non-negative seed
    if (argc == 3) {
        if (atoi(arguments[2]) < 0) {
            fprintf(stderr, "seed must be non-negative.\n");
            exit(3);
        }
    }
}



/******************************* checkMaxPlayer *******************************/  
/*
 * returns the index of the first slot that has its active==false
 * if no such index found, return negative, denoting that all slots are full
 */
int 
checkMaxPlayers(client_t *clientArray[])
{

    for (int i = 0; i < MaxPlayers; i++) {
        if (clientArray[i]->active == false) {
            return i; // empty spot available
        }
    }

    // no empty spots available
    return -1;
}


/******************************* removeAllPlayers *******************************/  
/*
 * free() the space allocated for the name of all players
 */
void 
removeAllPlayers()
{
    //loop through the players and free any data assocaited with them
    for (int i=0; i < MaxPlayers; i++) {
        if (clientArray[i]-> name != NULL) {
            free(clientArray[i]->name);
        }
        free(clientArray[i]->map);
    }

    //make sure to free spectator
    free(clientArray[MaxPlayers]->map);
}


/******************************* removePlayer *******************************/  
/*
 * free() the space allocated for the name of a player
 * assign the id of clientArray[index] to '!'
 */
void 
removePlayer(int index)
{
    //makes the id a bad number and quits the player
    clientArray[index]->id = 33;
    handleQuitMessage(clientArray[index]->address);
}

/******************************* addPlayer *******************************/  
/*
 * adds the player to the game struct
 */
bool 
addPlayer(const char *username, const addr_t from, int index)
{
    //mallocs name for the user
    clientArray[index]-> name = (char *)malloc((strlen(username)+1)*sizeof(char));
    // defensive code, in case there are problems allocating memory for 'name'
    if (clientArray[index]->name == NULL) {
        return false;
    }
    //sets default active values for the player
    strcpy(clientArray[index]->name, username);
    clientArray[index]-> address = from;
    clientArray[index]-> active = true;
    clientArray[index]-> score = 0;
    return true;
}

/******************************* addSpectator *******************************/  
/*
 * adds a spectator to the game struct
 */
void 
addSpectator(const addr_t from)
{
    clientArray[MaxPlayers]-> address = from;
    clientArray[MaxPlayers]->id = 111; // 'o' for occupied
}

/******************************* removeSpectator *******************************/  
/*
 * removes a spectator to the game struct
 */
void 
removeSpectator()
{
    message_send(clientArray[MaxPlayers]->address, "QUIT");
    clientArray[MaxPlayers]->id = 117; // 'u' for unoccupied
}

/******************************* isSpectatorAvailable *******************************/  
/*
 * checks if the spectator currently exists or not
 */
bool 
isSpectatorAvailable()
{
    if (clientArray[MaxPlayers]->id != 111) {
        return true;
    }
    return false;
}


/******************************* replaceSpectator *******************************/  
/*
 * sends a QUIT message to the original spectator
 * assigns new_spectator address as the address of clientArray[MaxPlayers]
 */
void 
replaceSpectator(const addr_t new_spectator)
{
    message_send(clientArray[MaxPlayers]-> address, "QUIT");
    clientArray[MaxPlayers]-> address = new_spectator;
    clientArray[MaxPlayers]->id = 111;
}

/******************************* handleGridMessage *******************************/  
/*
 * sends the length of each row and column to the user
 */
void 
handleGridMessage(const addr_t from)
{
    char resp[MaxBytes];
    sprintf(resp, "GRID %d %d",mapGetRows(),mapGetColumns());
    message_send(from, resp);
}

/******************************* handleGolddMessage *******************************/  
/*
 * sends the n p and r values to each user 
 */
void 
handleGoldMessage(const addr_t from, int n, int p, int r)
{
    char resp[MaxBytes];
    sprintf(resp, "GOLD %d %d %d",n,p,r);
    message_send(from, resp);
}

/******************************* handleGolddMessageAll *******************************/  
/*
 * calls handleGoldMessage in a loop and therefore sends gold messages 
 */
void 
handleGoldMessageAll(int index, int n, int p, int r)
{

    //for each player
    for (int i=0; i<MaxPlayers ; i++) {
        //if they are active
        if (clientArray[i]->active) {

            //send the specific user a different gold message based on what they collect4ed
            if (i==index) {
                handleGoldMessage(clientArray[i]->address,n,p,r);
            }

            //else send everyone 0 collected
            else {
                handleGoldMessage(clientArray[i]->address,0,clientArray[i]->score,r);
            }
        }
    }

    // for spectator, n and p are always 0
    if (clientArray[MaxPlayers]->id == 111) {
        handleGoldMessage(clientArray[MaxPlayers]->address,0,0,r);
    }   
}

/******************************* handleQuitMessage *******************************/  
/*
 * sends a quit message to the input user
 */
void 
handleQuitMessage(const addr_t from)
{
    message_send(from, "QUIT");
}

/******************************* handleGameovermessage *******************************/  
/*
 * sends a game over message to each user - contains the summary string with data
 * and results from the game that just ended
 */
void 
handleGameoverMessage()
{
    char* resp = malloc(MaxBytes * sizeof(char));
    char* summary;
    int m =0;
    int placeholder = 0;
    //for each player, count up the bytes that would have to be added to a string 
    for (int i = 0; i< MaxPlayers; i++) {
        if (clientArray[i]->active==true) {
            char* nuggets = malloc(50 * sizeof(char));
            sprintf(nuggets, "%d", clientArray[i]-> score);
            m = m + strlen(clientArray[i]-> name) + strlen(nuggets) + 6;
            free(nuggets);
        }

    }

    //allocate memory for that amount of bytes plus 1 for the null terminating character
    summary = malloc((m+1)*sizeof(char));
    //formulating the summary message
    for (int i = 0; i< MaxPlayers; i++) {

        char character = i+65;
        // we only check if it has EVER been active as it also includes people that have left the game 
        if (clientArray[i]->active) {
            // converts the score to string and stores in nuggets
            char* nuggets = malloc(50 * sizeof(char));
            sprintf(nuggets, "%d", clientArray[i]-> score);
            sprintf(summary + placeholder, "%c, %s, %s\n", character, nuggets, clientArray[i]->name);
            placeholder = placeholder + strlen(nuggets) + strlen(clientArray[i]->name) + 6;
            free(nuggets);
        }

    }

    //store GAMEOVER\nsummary in resp
    sprintf(resp, "GAMEOVER\n%s",summary);

    //send resp to all players
    for (int i = 0; i< MaxPlayers; i++) {
        //only send the message to players that have not left the game
        if (clientArray[i]->active==true && clientArray[i]->id!=33) {
            message_send(clientArray[i]->address, resp);
        }
    }

    //only send the resp to spectator if the spectator spot is taken 
    if (isSpectatorAvailable()==false) {
        message_send(clientArray[MaxPlayers]->address, resp);
    }

    free(resp);
    // free memory allocated for summary
    free(summary);
    // free memory allocated for all the player names
    removeAllPlayers();

}

/******************************* findClient *******************************/  
/*
 * based on an inputId, the method will return the client associated with it
 */
client_t* 
findClient(char inputId)
{
    //loop through the array of all the characters and return the correct client
    for (int i = 0; i < MaxPlayers; i++) {
        if (clientArray[i]->id == inputId) {
            return clientArray[i];
        }
    }
    return NULL;
}

/******************************* parseMessage *******************************/  
/*
 * this is the meat of the program - will constantly loop through to find messages
 * until this funtion returns true. Decodes each message and calls functionality
 * across the server.c, game.c and map.c module
 */
bool 
parseMessage (void *arg, const addr_t from, const char *message) 
{
    game_t* thisGame = (game_t*)arg;

    // if it is a spectator
    if (strncmp(message,"SPECTATE",8) == 0) {

        // if there is spectator spot available
        if (isSpectatorAvailable()) {
            addSpectator(from);
        }
        // if there is already a spectator, this spectator takes its place
        else {
            replaceSpectator(from);
        }
        handleGridMessage(from);
        handleGoldMessageAll(MaxPlayers, 0, 0, thisGame->nuggetsLeft);
        updatePlayers(thisGame);
    }

    // else it is a player
    else {

        const char separator = ' ';
        // PLAY message
        if (strncmp(message,"PLAY ",5) == 0) {
            int index;

            // check if there are already MaxPlayers clients
            if ((index = checkMaxPlayers(clientArray)) < 0) {
                printf("did we get here\n");
                message_send(from, "NO");
            }

            else {
                // extract "real name" of the client
                char * realName = strchr(message, separator); // gets everything after (and including) the space
                realName = realName+1; // removing the first space character
                if (addPlayer(realName, from, index)) {
                    char response[MaxBytes];
                    sprintf(response, "OK %c",clientArray[index]->id);
                    
                    //if the player is active
                    if (clientArray[index]->active) {
                        //initialize the current position of the player and put it in the masterMap
                        int temp = clientArray[index]->position.x;
                        int pos = mapGetColumns()*(clientArray[index]->position.y - 1) + clientArray[index]->position.x - 1;
                        clientArray[index]->position.x = -1;
                        //places the player
                        updateMap(thisGame->masterMap, pos, clientArray[index]);
                        clientArray[index]->position.x = temp;
                    }
                    
                    //sends the starting messages
                    message_send(from, response);
                    handleGridMessage(from);
                    handleGoldMessageAll(index, 0, 0, thisGame->nuggetsLeft);

                    //updates client maps for everyone
                    updatePlayers(thisGame);
                }
            }
        }

        // Keystroke message
        if (strncmp(message,"KEY ",4)==0){

            client_t *temporaryClient;
            int clientIndex = -1;
            
            // checking if the player already exists by
            // comparing address of 'from' against addresses of elements of clientArray[]
            for (int i = 0; i < MaxPlayers+1; i++) {
                if (message_eqAddr(clientArray[i]->address, from)) {
                    temporaryClient = clientArray[i];
                    clientIndex = i;
                    break;
                }
            }
            
            // clientIndex will be non-negative if the address of the current client matches
            // the address of a client that is already in clientArray[]
            if (clientIndex >= 0) {
                int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;

                char * Keystroke = strchr(message, separator);
                Keystroke = Keystroke + 1;

                //if the keystroke is quit
                if (strcmp(Keystroke, "Q") == 0) {
                    //remove the spectator if they quit
                    if (clientIndex == MaxPlayers) {
                        removeSpectator();
                    }
                    //remove the player, and change the value of their position on the map
                    else {

                        //if the player is in a pathway, update the map accordingly
                        if (checkIfInPathway(pos)) {
                            thisGame->masterMap[pos] = '#';
                        }
                        //else make it just an empty spot
                        else {
                            thisGame-> masterMap[pos] = '.';
                        }
                        //remove the player
                        removePlayer(clientIndex);
                    }
                }

                else if (strcmp(Keystroke, "h") == 0) {
                    //call moveLeft in the game
                    goldOrId_t* data = moveLeft(temporaryClient,thisGame);

                    //if it was successful
                    if (data != NULL) {

                        //if there was gold collectec
                        if(data->gold > 0){

                            //send out a message to everyone
                            handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            //update the position of the client
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                            //make the old position of the client what it should be in the masterMap
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos+1] = thisGame->masterMap[pos+1];
                        }

                        //else if the player moved to an empty space
                        else if (data->id == 'w') {
                            //update its position
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                            //update its old position
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos+1] = thisGame->masterMap[pos+1];
                        }

                        //else if the player ran into another player
                        else if (data->id != '\0') {
                            //find the client that it ran into
                            client_t* toChange = findClient(data->id);
                            //update the position of the client that got changed
                            toChange->position.x = toChange->position.x + 1;
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                        }
                    }

                    //update everyones individualized map
                    updatePlayers(thisGame);
                    free(data);
                }
                //same as moveleft, but only moving right
                else if (strcmp(Keystroke, "l") == 0) {
                    //call moveRight in the game
                    goldOrId_t* data = moveRight(temporaryClient,thisGame);

                    //if the move was successful
                    if (data != NULL) {

                        //if gold was run into
                        if(data->gold > 0){
                            handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos-1] = thisGame->masterMap[pos-1];
                        }

                        //if moved into an empty spot
                        else if (data->id == 'w') {
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos-1] = thisGame->masterMap[pos-1];
                        }

                        //if ran into another client
                        else if (data->id != '\0') {
                            client_t* toChange = findClient(data->id);
                            toChange->position.x = toChange->position.x - 1;
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                        }
                    }
                    updatePlayers(thisGame);
                    free(data);
                }

                //same as moveLeft, but moving down
                else if (strcmp(Keystroke, "j") == 0) {
                    //call moveDown in game.c
                    goldOrId_t* data = moveDown(temporaryClient,thisGame);
                    if (data != NULL) {

                        if(data->gold > 0){
                            handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos - mapGetColumns()] = thisGame->masterMap[pos - mapGetColumns()];
                        }

                        else if (data->id == 'w') {
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos - mapGetColumns()] = thisGame->masterMap[pos - mapGetColumns()];
                        }

                        else if (data->id != '\0') {
                            client_t* toChange = findClient(data->id);
                            toChange->position.y = toChange->position.y - 1;
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                        }
                    }

                    updatePlayers(thisGame);
                    free(data);
                }

                //same as moveLeft, but moving up
                else if (strcmp(Keystroke, "k") == 0) {
                    goldOrId_t* data = moveUp(temporaryClient,thisGame);

                    if (data != NULL) {

                        if (data->gold > 0){
                            handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos + mapGetColumns()] = thisGame->masterMap[pos + mapGetColumns()];
                        }

                        else if (data->id == 'w') {
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos + mapGetColumns()] = thisGame->masterMap[pos + mapGetColumns()];
                        }

                        else if (data->id != '\0') {
                            client_t* toChange = findClient(data->id);
                            toChange->position.y = toChange->position.y + 1;
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                        }
                    }

                    updatePlayers(thisGame);
                    free(data);
                }

                //same as moveLeft, but moving up and left
                else if (strcmp(Keystroke, "y") == 0) {

                    goldOrId_t* data = moveUpAndLeft(temporaryClient,thisGame);

                    if (data != NULL) {

                        if(data->gold > 0){
                           handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos + mapGetColumns() + 1] = thisGame->masterMap[pos + mapGetColumns() + 1];
                        }

                        else if (data->id == 'w') {
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos + mapGetColumns() + 1] = thisGame->masterMap[pos + mapGetColumns() + 1];
                        }

                        else if (data->id != '\0') {
                            client_t* toChange = findClient(data->id);
                            toChange->position.y = toChange->position.y + 1;
                            toChange->position.x = toChange->position.x + 1;
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                        }
                    }

                    updatePlayers(thisGame);
                    free(data);
                }

                //same as moveLeft, but moving up and right
                else if (strcmp(Keystroke, "u") == 0) {

                    goldOrId_t* data = moveUpAndRight(temporaryClient,thisGame);

                    if (data != NULL) {

                        if(data->gold > 0){
                            handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos + mapGetColumns() - 1] = thisGame->masterMap[pos + mapGetColumns() - 1];
                        }

                        else if (data->id == 'w') {
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos + mapGetColumns() - 1] = thisGame->masterMap[pos + mapGetColumns() - 1];
                        }

                        else if (data->id != '\0') {
                            client_t* toChange = findClient(data->id);
                            toChange->position.y = toChange->position.y + 1;
                            toChange->position.x = toChange->position.x - 1;
                            temporaryClient->position.y = temporaryClient->position.y - 1;
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                        }
                    }

                    updatePlayers(thisGame);
                    free(data);
                }

                //same as moveLeft, but moving down and left
                else if (strcmp(Keystroke, "b") == 0) {

                    goldOrId_t* data = moveDownAndLeft(temporaryClient,thisGame);
                    if (data != NULL) {

                        if(data->gold > 0){
                            handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos - mapGetColumns() + 1] = thisGame->masterMap[pos - mapGetColumns() + 1];
                        }

                        else if (data->id == 'w') {
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos - mapGetColumns() + 1] = thisGame->masterMap[pos - mapGetColumns() + 1];
                        }

                        else if (data->id != '\0') {
                            client_t* toChange = findClient(data->id);
                            toChange->position.y = toChange->position.y - 1;
                            toChange->position.x = toChange->position.x + 1;
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            temporaryClient->position.x = temporaryClient->position.x - 1;
                        }
                    }

                    updatePlayers(thisGame);
                    free(data);
                }

                //same as moveLeft, but moving down and right
                else if (strcmp(Keystroke, "n") == 0) {

                    goldOrId_t* data = moveDownAndRight(temporaryClient,thisGame);

                    if (data != NULL) {

                        if(data->gold > 0){
                            handleGoldMessageAll(clientIndex, data->gold, clientArray[clientIndex]->score, thisGame->nuggetsLeft);
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos - mapGetColumns() - 1] = thisGame->masterMap[pos - mapGetColumns() - 1];
                        }

                        else if (data->id == 'w') {
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                            int pos = ((temporaryClient->position.y - 1) * mapGetColumns()) + temporaryClient->position.x - 1;
                            temporaryClient->map[pos - mapGetColumns() - 1] = thisGame->masterMap[pos - mapGetColumns() - 1];
                        }

                        else if (data->id != '\0') {
                            client_t* toChange = findClient(data->id);
                            toChange->position.y = toChange->position.y - 1;
                            toChange->position.x = toChange->position.x - 1;
                            temporaryClient->position.y = temporaryClient->position.y + 1;
                            temporaryClient->position.x = temporaryClient->position.x + 1;
                        }
                    }

                    updatePlayers(thisGame);
                    free(data);
                }

                //if the user inputs a random keystroke
                else {
                  message_send(from, "NO unrecognized keystroke");
                }

                //if there are no nuggets left, the gam ei sover
                if (thisGame->nuggetsLeft == 0) {
                    //send a game over message to each client
                    handleGameoverMessage();
                    free(thisGame->masterMap);
                    free(thisGame);
                    return true; //ends the loop
                }
            }
        }
    }
    //continues the loop
    return false;
}

void
updatePlayers(game_t* game)
{

    //sends DISPLAY messages to the spectator if its id is 'o', denoting that it is occupied
    if (clientArray[MaxPlayers]->id==111) {
        char* masterMap = malloc((9+mapGetRows()*mapGetColumns())*sizeof(char));
        sprintf(masterMap, "DISPLAY\n%s", game->masterMap);    
        message_send(clientArray[MaxPlayers]->address, masterMap);
        free(masterMap);
    }

    //sends DISPLAY messages to the players if they are active
    for (int i = 0; i < MaxPlayers; i++) {

        // only send DISPLAY messages to players that are active and players that have not left the game
        if (clientArray[i]->active==true && clientArray[i]->id!=33) {
            visibility(game->masterMap, clientArray[i]);
            char* string = malloc((9+mapGetRows()*mapGetColumns())*sizeof(char));
            sprintf(string, "DISPLAY\n%s", clientArray[i]->map);
            message_send(clientArray[i]->address, string);
            free(string);
        }
    }
}
