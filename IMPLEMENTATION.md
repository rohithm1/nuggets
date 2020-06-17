# Implementation Spec for the Nuggets Game

## Detailed pseudo code for each of the objects/components/functions

### Game Module

`game_t* gameNew(char* filename, int rand);`:
1. mallocs enough memory to hold a `game` struct
2. make a new `map` by calling the `map` module
3. gets the total rows and cols for the `masterMap`
4. calculate the amount of piles using the random integer
5. while there are still piles to be added
	1. if the random location in the map is empty, place a gold
	2. else go to the next location
6. return the game

`bool addClient(client_t* client, game_t* game); `
1. get the number of rows and cols
2. get a random index in the map
3. while the random index is not empty
	1. if its empty, end the while loop
	2. else change the index to the next possible index
4. initialize map location of the player and the actual map
5. return true

`goldOrId_t* moveLeft(client_t* client, game_t* game);`
1. grab the current position of the client
2. get the total amount of rows and cols by calling a `map` function
3. calculate the index version of the client using the rows and cols
4. initialize a new instance of `goldId`
5. if the index is greater than or equal to 0
	1. if the character returned from update position is not empty
		1. if that character is 'g'
			1. determine the amount of gold the pile is worth
			2. subtract the pile from the total amount of piles
			3. calculate the new number for total gold left
			4. update player score
			5. return `goldId`
		2. else if the character is 'w'
			1. set `int value` to 0 and `id` to '\0'
			2. return 	`goldId`
		3. else if the character is an id (`char` A-Z)
			1. set `int value` to 0 and `id` to the `temp` char read in
			2. return `goldId`
6. return -1
*note the function is return type int to tell the server that both a gold pile has been found, and the value of that gold pile*

*The following functions same futionality/details/pseudocode as moveleft, but they just move in a different direction, so the implementation will be different*
`goldOrId_t* moveRight(client_t* client, game_t* game);`: moves right instead of left
`goldOrId_t* moveUp(client_t* client, game_t* game);`: moves up instead of left
`goldOrId_t* moveDown(client_t* client, game_t* game);` : moves down instead of left

*The following functions call some combination of moveLeft, moveRight, moveUp and moveDown*
`goldOrId_t* moveUpAndLeft(client_t* client, game_t* game);`: calls moveLeft and moveUp to move diagnally
`goldOrId_t* moveUpAndRight(client_t* client, game_t* game);`: calls moveRight and moveUp to move diagnally
`goldOrId_t* moveDownAndRight(client_t* client, game_t* game);`: calls moveDown and moveRight to move diagnally
`goldOrId_t* moveDownAndLeft(client_t* client, game_t* game);`: calls moveDown and moveRight to move diagnally

`int goldValue(game_t* game);`
1. if there is only 1 pile left
	1. return the gold that is left
2. if the maxValue is greater than 30
	1. randomize an integer from 0 to 30
3. else
	1. randomize an integer from 0 to maxValue
4. return the value

### Map Module
`char* mapNew(char* filename)`
1. try opening the file, give an error message and return NULL if not possible
2. read a line from the file to find the number of characters in a line, which becomes number of *columns* in our map
3. find the number of *rows* using the `lines_in_file` function from `file.h`
4. close the file and reopen it to start reading from the beginning
5. make an array that is the *map*
6. use `fgetc` to get one character at a anytime
	1. if it is not the end of the file, add the character to the *map*
7. set the global variable *original_map* equal to *map* and return it
8. if no file was given, create the client *map*

`char updateMap(char *map, int new_idx, client_t client)`
1. find the client's position before it is moved
2. check what is at the new index
	1. if it is empty (. or #), simply make the new_index the client id (A-Z), edit the original position with the *original_map*, and return 'w'
	2. if there is gold (\*), make the new index the client id, edit the original position with the *original_map*, change the new position on the *original_map* to empty, and return 'g'
	3. if there is a client (A-Z), switch the positions of the two clients on the map (without changing the client's position variable) and return the id of the other client so the game knows to update the position of the other module

`void visibility(char *map, client_t* client)`
1. count how many steps rows/columns you can go up/down/left/right at a certain position
2. set everything in the horizontal and vertical to visible by adding it to an array (until it hits a wall)
3. go through every possible slope in each of the 4 quadrants to add to the array of visible locations (until you hit a wall)
4. figures out the walls surrounding the area of visibility
5. makes the current client's letter '@'

`bool addGold(char *map, int idx) `
1. if the index given is empty, add the gold "\*" to both the *map* it is given as a parameter and the *original_map*
2. return true if the gold was added, false otherwise

`int nextToWall(int visible[], int index, int emptySpot)`
1. return true if a certain position is next to a wall
2. return false otherwise

`bool checkIfClientPosValid(char *map, int idx)`
1. return true if the position is somewhere that the player can move
2. return false otherwise

`bool checkIfGold(char *map, int idx)`
1. return true if the position has gold
2. return false otherwise

`bool checkIfClient(char *map, int idx) `
1. return true if the position has a client on it
2. return false otherwise

`bool checkIfEmpty(char *map, int idx)`
1. return true if the position is empty (.)
2. return false otherwise

`bool checkIfWall(char *map, int idx)`
1. return true if the position is a wall (+ or | or -)
2. return false otherwise

`bool checkIfTunnel(char *map, int idx)`
1. return true if the position is a tunnel (#)
2. return false otherwise

`bool checkIfMovable1(char* map, int move, int north, int east)`
1. return true if we can continue moving at a certain slope in quadrant 1
2. return false otherwise

`bool checkIfMovable2(char* map, int move, int north, int west)`
1. return true if we can continue moving at a certain slope in quadrant 2
2. return false otherwise

`bool checkIfMovable3(char* map, int move, int south, int west)`
1. return true if we can continue moving at a certain slope in quadrant 3
2. return false otherwise

`bool checkIfMovable4(char* map, int move, int south, int east)`
1. return true if we can continue moving at a certain slope in quadrant 4
2. return false otherwise

`int mapGetRows()`
1. return the number of rows

`int mapGetColumns()`
1. return the number of columns

`void deleteMap(char *map)`
1. frees the map


### Client
`main`
1. The main function first checks that the arguments fit the format `hostname, port, [playername]`, if not, the programs quits and logs a message to stderr.
	1. It mallocs the space for the `hostname`, the `port`, and the `playername` if it exists
2. Next it initializes the ncurses library and finds the dimensions of the screen.
	1. it saves the dimensions of the screen in to `screenHeight` and `screenWidth`
3. Next, `message_setAddr` is called to fill in the `addr_t` struct needed to communicate with the server.
4. It then initializes the `message` module to connect to the `server`. It sends the `server` a message requesting that this client join the game.
	1. if there is no `playername`, then it attempts to join as a spectator
5. It next initializes the `message_loop` that allows the client to keep sending messages to and receiving messages from the server.
	1. calls `updateServer` as the 4th argument of `message_loop` with communicates with the server
	2. calls `parseMessage` as the 5th argument of the `message_loop`
6. After the loop ends `main` calls `leaveGame`, which frees all memory that was malloc'ed

`void checkScreenSize()`
1. compares `ncols` and `nrows` to `screenWidth` and `screenHeight`, checking that the user's screen is not too small. If it is it outputs a message on the status line.

`bool readUserInput()`
1. this function uses the `ncurses` library to read in whatever key char c was called by the user.
2. then concatenate the strings "KEY " and the character that was read in, and send that message to the server through the function `updateServer`

`void updateServer(addr_t to, char* message)`
1. Simply calls `message_send` to send the desired message to the server.

`void printLog(char* message);`
1. prints desired message to stderr

`void displayMap(char* map)`
Uses the `ncurses` libary.
1. first displays the status message `displayMessage`
2. Then it goes through each character in the string sent in from the server. Since `nrows` and `ncols` are stored as global variables, it simply loops through each row adding in the next char of the line. It then adds a new line character at the end of each row. It does this for all `ncols` columns.
3. it then calls `refresh`

`bool parseMessage(void *arg, const addr_t from, const char* message)`
this function is called as a part of the `message_loop`.
1. The server can return the following statements:
	1. "NO"
		1. if this message is followed by a message, then this means that too many players are already playing the game. Then this message is logged to the user and the loop is terminated
		2. otherwise the server did not understand the client's message, this is logged to the user and the loop is not ended
	2. "OK"
		1. this means that the user was allowed into the game. It then updates the displayMessage to tell the user their user ID that will appear on the screen.
	3. "DISPLAY"
		1. This message is followed by a string which represents a map. This means that `displayMap` is immediately called.
	4. "GRID"
		1. this is followed by the dimensions of the Grid, these values are saved into `ncols` and `nrows`. `checkScreenSize` is then called.
	5. "GOLD"
		1. This message is followed by a string which tells the client how many nuggets they just collected, what their current score is, and how many nuggets are left to be found. It then updates the display message.
	6. "GAMEOVER"
		1. This message is followed by a string containing the final results for all players. This message is printed out line by line onto the screen for the user to see. Then the loop is terminated.
2. returns false on default for any message that does not require a `leaveGame` call immediately afterwords.
	1. if this is the case, `parseMessage` returns true, which will terminate `message_loop`.

`void leaveGame();`
1. ends the `ncurses` library.
2. frees all global variables that were malloc'ed.


### Player
`main`
1. The main function first checks that the arguments fit the format `hostname, port, [playername]`, if not, the programs quits and logs a message to stderr.
	1. It mallocs the space for the `hostname`, the `port`, and the `playername` if it exists
2. Next it initializes the ncurses library and finds the dimensions of the screen.
	1. it saves the dimensions of the screen in to `screenHeight` and `screenWidth`
3. Next, `message_setAddr` is called to fill in the `addr_t` struct needed to communicate with the server.
4. It then initializes the `message` module to connect to the `server`. It sends the `server` a message requesting that this client join the game.
	1. if there is no `playername`, then it attempts to join as a spectator
5. It next initializes the `message_loop` that allows the client to keep sending messages to and receiving messages from the server.
	1. calls `updateServer` as the 4th argument of `message_loop` with communicates with the server
	2. calls `parseMessage` as the 5th argument of the `message_loop`
6. After the loop ends `main` calls `leaveGame`, which frees all memory that was malloc'ed

`void checkScreenSize()`
1. compares `ncols` and `nrows` to `screenWidth` and `screenHeight`, checking that the user's screen is not too small. If it is it outputs a message on the status line.
2. Since ncurses automatically quits upon a `window resize ` signal, `checkScreenSize()` uses a helper function to catch that call initialize a new screen
3. Continue the screen resizing process until the user adjusts the window to the proper size.

`bool readUserInput()`
1. this function uses the `ncurses` library to read in whatever key char c was called by the user.
2. then concatenate the strings "KEY " and the character that was read in, and send that message to the server through the function `updateServer`

`void updateServer(addr_t to, char* message)`
1. Simply calls `message_send` to send the desired message to the server.

`void printLog(char* message);`
1. prints desired message to stderr

`void displayMap(char* map)`
Uses the `ncurses` libary.
1. first displays the status message `displayMessage`
2. Then it goes through each character in the string sent in from the server. Since `nrows` and `ncols` are stored as global variables, it simply loops through each row adding in the next char of the line. It then adds a new line character at the end of each row. It does this for all `ncols` columns.
3. it then calls `refresh`

`bool parseMessage(void *arg, const addr_t from, const char* message)`
this function is called as a part of the `message_loop`.
1. The server can return the following statements:
	1. "NO"
		1. if this message is followed by a message, then this means that too many players are already playing the game. Then this message is logged to the user and the loop is terminated
		2. otherwise the server did not understand the client's message, this is logged to the user and the loop is not ended
	2. "OK"
		1. this means that the user was allowed into the game. It then updates the displayMessage to tell the user their user ID that will appear on the screen.
	3. "DISPLAY"
		1. This message is followed by a string which represents a map. This means that `displayMap` is immediately called.
	4. "GRID"
		1. this is followed by the dimensions of the Grid, these values are saved into `ncols` and `nrows`. `checkScreenSize` is then called and `currMap` is malloc'ed
		2. Uses a helper function `readGridMessage` to parse the content of the method and cast the numbers from a string to an integer
	5. "GOLD"
		1. This message is followed by a string which tells the client how many nuggets they just collected, what their current score is, and how many nuggets are left to be found. It then updates the display message.
		2. uses a helper function `readGoldMessage` to parse the contents of the string and save the contents of the message into `nugsLeft` and `purse` and to create the appropriate display message
	6. "GAMEOVER"
		1. This message is followed by a string containing the final results for all players. This message is printed out line by line onto the screen for the user to see. Then the loop is terminated.
2. returns false on default for any message that does not require a `leaveGame` call immediately afterwords.
	1. if this is the case, `parseMessage` returns true, which will terminate `message_loop`.

`void leaveGame();`
1. ends the `ncurses` library.
2. frees all global variables that were malloc'ed.




### Server

`int main(int argc, char *argv[])`
1. Calls validate_arguments() to check for correct arguments
2. Initializes message module
3. Creates and initializes a *game_t* variable by calling gameNew()
4. Displays a ready message alongwith the port number
5. Allocate memory for elements of client_array[] and initialize id values and active flags
6. Call updatePlayers(game)
7. Call message_loop()
8. free(client_array[i]) for all elements of client_array[]
9. Call message_done()

`void validate_arguments(int no_of_args, char *arguments[])`
1. if number of arguments is not 2 and number of arguments is not 3:
	1. Display error message and exit non-zero
2. if map.txt does not exist or is not readable:
	1. Display error message and exit non-zero
3. if(no_of_args==3):
	1. Check for non-negative seed
		1. Display error message and exit non-zero

`int check_maxplayers(client_t *client_array[])`
1. Iterate over the array of 26 players
	1. if (client_array[i]->active==false):
		1. return i
2. return -1

`void remove_all_players()`
1. Iterate over the array of 26 players
    1. if(client_array[index]-> name != NULL):
        1. free(client_array[index]-> name);
    2. free(client_array[i]->map)
2. free(client_array[26]->map)

`void remove_player(int index)`
1. client_array[index]-> id = 33
2. handleQuitMessage(client_array[index]->address);

`bool add_player(const char *username, const addr_t from, int index)`
1. Allocate memory for the client_array[index]->name
2. if(client_array[index]->name == NULL):
	1. return false
3. Initialize name, address, active and score of client_array[index]
4. return true

`void add_spectator(const addr_t from)`
1. Assigns address of the last element of client_array to the address passed as an argument.
2. Assigns the id of the last element of client_array to 'o'

`void remove_spectator()`
1. message_send(client_array[26]->address, "QUIT");
2. Assigns the id of the last element of client_array to 'u'

`bool is_spectator_available()`
1. if the id of the last element of client_array is not 'o':
	1. return true
2. return false

`void replace_spectator(const addr_t new_spectator)`
1. Send a "QUIT" message to address at client_array[26]->address
2. client_array[26]-> address = new_spectator
3. The id of the last element of client_array is assigned to 'u'

`void handleGridMessage(const addr_t from)`
1. Send a "GRID" message to address passed as argument

`void handleGoldMessage(const addr_t from, int n, int p, int r)`
1. Send a "GOLD" message to address passed as argument

`void handleGoldMessageAll(int index, int n, int p, int r)`
1. Iterate over all clients except for the last one:
    1. if (client_array[i]->active):
        1. if(i==index):
            1. call handleGoldMessage(client_array[i]->address,n,p,r) to handle the current player
        2. else:
            1. call handleGoldMessage(client_array[i]->address,0,client_array[i]->score,r) to handle all other players

2. if((client_array[26]->id == 111)):
    1. call handleGoldMessage(client_array[i]->address,0,client_array[i]->score,r) to handle the spectator

`void handleDisplayMessage(const addr_t from)`
1. Send a "DISPLAY" message to address passed as argument

`void handleQuitMessage(const addr_t from)`
1. Send a "QUIT" message to address passed as argument

`void handleGameoverMessage(const addr_t from)`
1. Calculate memory to be allocated for storing the summary message
2. Allocate memory for summary
3. Iterate over all players:
    1.  if(client_array[i]->active):
        1. Append the playerID, score and name to summary
2. Send a "GAMEOVER\nsummary" message to all active players and spectator
3. free() memory allocated for summary and response
4. Call remove_all_players()

`client_t* find_client(char input_id)`
1. Iterate over all players in client_array[]:
	1. if (client_array[i]->id == input_id):
		1. return client_array[i]
2. return NULL

`void updatePlayers(game_t* game)`
1. if (client_array[26]->id==111)
    1. Allocate memory for string_mastermap
    2. Create the DISPLAY message with game->masterMap
    3. message_send(client_array[26]->address, string_mastermap);
    4. free(string_mastermap);

2. Iterate over al players:
    1. if (client_array[i]->active==true && client_array[i]->id!=33):
        1. visibility(game->masterMap, client_array[i]);
        2. Create the DISPLAY message with client_array[i]->map
        3. message_send(client_array[26]->address, string);
        4. free(string);

`bool parseMessage(void *arg, const addr_t from, const char* message)`
this function is called as a part of the `message_loop`.
1. The server can get the following messages:
    1. "PLAY __*username*__"
        1. if(there are max_players):
            1. Respond with a "NO". This is a response to a client that tries to join a game that has MaxPlayers playing it.

        2. else:
		    1. This means that the user was allowed into the game.
            2. It saves the name of the player, sends an "OK playerID" message, followed by grid and gold messages followed by updatePlayers().

    2. "SPECTATE"
        1. if there is spectator spot available:
            1. add_spectator(from)
        2. else:
            1. replace_spectator(from)
        3. handleGridMessage() followed by handleGoldMessageAll() and updatePlayers()

    3. "KEY __*k*__"
        1. Checking if the player already exists by comparing address of 'from' against addresses of elements of clientArray[]
        2. Check if keystroke is a recognized keystroke:
            1. Call the appropriate move command based on the keystroke
            2. if gold was collected:
                1. handleGoldMessageAll()
                    1. This message is followed by a string which tells the client how many nuggets they just collected, what their current score is, and how many nuggets are left to be found. This message is sent whenever a player collects gold, resulting in the total number of nuggets left to change. For the spectator, n and p will always be 0.
                2. update position
            3. else if the player walked into an empty spot:
                1. update position
            4. else if the player ran into another client:
                1. swap positions
            5. updatePlayers(this_game)
                1. This message is sent after every incoming keystroke as well as when a new spectator/player is added to the game. The "DISPLAY" is followed by a string which represents a map.
            6. free data allocated  

        3. else, the server sends "NO unrecognized keystroke" message to the client when the keystroke is not understandable.
        4. if (this_game->nuggetsLeft == 0):
            1. handleGameoverMessage()
                1. This message is followed by a string containing the final results for all players that have ever played the game, even if they have quit before the end of the game. It will be in the following format: "playerID, nuggets, playerName"  
            2. free allocated memory
            3. return true to break the loop

## Definition of detailed APIs, interfaces, function prototypes and their parameters

### Player
The `player` module uses the following global functions to connect to the server, parse server messages, display the map and status line, and send messages regarding player movement to the server.
``` c
void checkScreenSize();
bool readUserInput();
bool parseMessage(void *arg, const addr_t from, const char* message);
void printLog(char* message);
void updateServer(addr_t to, char* message);
void leaveGame();
void displayMap();
```
To help with readability, longer functions such as parseMessage or checkScreenSize are split into different local helper functions.

```c
static void readGoldMessage(char* content);
static void readGridMessage(char* content);
static void initializeCurses();
static void doResize();
```
Additionally, the `player` uses the following global variables to keep track of states that are important to gameplay.

```c
static const int MaxNameLength = 50;   // max number of chars in playerName
char* hostname; //host name of the server
char* port; //server port
addr_t *address; //server address
char* playername; //player name inputted by the user

int screenHeight, screenWidth; //dimensions of the screen
static int ncols, nrows; //number of columns and rows of the map

char* displayMessage; //the status message displayed above the map

char* extraNotification;
bool displayExtraNotification = false;

char id; //id for the client as provided by the server
char* purse; //the client's current score
char* nugsLeft; // how many nuggets are left to be found in the game

bool isSpectator = false; //whether or not this client is a spectator

char* currMap; // the current map being displayed as passed down from the server

bool screenSizeOK = false; //whether or not the screen is big enough
```

### Game Module
```c
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
```
`game_t* gameNew();`: will malloc a memory space and return a `game` struct for the server to use. The `filename` is the file that contains the map the server wants to use as the game, and `rand` is an integer used to calcuate gold piles and locations - randomized by the function
`bool addClient(client_t* client, game_t* game);`: will add the player to the game. `client` is the player to be added, and `game` that it is added to.
`bool moveLeft(client_t* client, game_t* game);`: will move the player left one space, `client` is the actual player calling the move, and `game` is the struct that holds the maps

*The following functions same functionality and details as moveleft, but they just move in a different direction, so the implementation will be different*
```c
goldOrId_t* moveRight(client_t* client, game_t* game);
goldOrId_t* moveUp(client_t* client, game_t* game);
goldOrId_t* moveDown(client_t* client, game_t* game);
goldOrId_t* moveUpAndLeft(client_t* client, game_t* game);
goldOrId_t* moveUpAndRight(client_t* client, game_t* game);
goldOrId_t* moveDownAndRight(client_t* client, game_t* game);
goldOrId_t* moveDownAndLeft(client_t* client, game_t* game);
```
Additionally, the game module uses the following global variables:
```c
static const int MaxBytes = 65507;     // max number of bytes in a message
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles
```
Here is the structure of the game module as well:
```c

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

```

### Map Module
The `map` module initiates the master map and makes changes to maps that are passed to it. It is directly called by the `game` module, and it has the following functions:
```c
char* mapNew(char* filename);
char updateMap(char *map, int new_idx, client_t client);
void visibility(char *map, client_t* client);
int nextToWall(int visible[], int index, int emptySpot);
bool addGold(char *map, int idx);
bool checkIfClientPosValid(char *map, int idx);
bool checkIfGold(char *map, int idx);
bool checkIfClient(char *map, int idx);
bool checkIfWall(char* map, int idx);
bool checkIfEmpty(char *map, int idx);
bool checkIfTunnel(char *map, int idx);
bool checkIfMovable1(char* map, int move, int north, int east);
bool checkIfMovable2(char* map, int move, int north, int west);
bool checkIfMovable3(char* map, int move, int south, int west);
bool checkIfMovable4(char* map, int move, int south, int east);
bool checkIfInPathway(int index);
int mapGetRows();
int mapGetColumns();
void deleteMap(char *map);
```

The `map` module has the following global variables to keep track of the original map that was read in from the .txt file:
```c
int columns;
int rows;
char *original_map;
```

### Server

The `server` initializes the `message` module and the `game` module before displaying the obtained port. It communicates with clients and stores their details in a global array of type `client_t`. It initializes this global array before calling *message_loop()* that waits for a message from the client and takes action according to the message received. The server makes use of the following functions:

```c
void remove_player(int index);
bool add_player(const char *username, const addr_t from, int index);
void add_spectator(const addr_t from);
void remove_spectator();
bool is_spectator_available();
void replace_spectator(const addr_t new_spectator);
bool parseMessage (void *arg, const addr_t from, const char *message);
void handleGridMessage(const addr_t from);
void handleGoldMessage(const addr_t from, int n, int p, int r);
void handleGoldMessageAll();
void handleDisplayMessage(const addr_t from);
void handleQuitMessage(const addr_t from);
void handleGameoverMessage(const addr_t from);
client_t* find_client(char input_id);
void updatePlayers(game_t* game);
void remove_all_players();
void validate_arguments(int no_of_args, char *arguments[]);
int check_maxplayers(client_t *client_array[]);
```

The following are some important structures that the server uses:
```c
typedef struct xy_coord{
    int x;
    int y;
} xy_coord_t;

typedef struct client{

    char id;
    char *name;
    addr_t address;
    xy_coord_t position;
    char *map;
    int score;
    bool active;

} client_t;
```

### Player interface
To initialize the `player` interface, the command line must receive
`./player hostname port [playername]`

The player interface will require implementation for a wide variety of keystrokes, as detailed in requirements

The player, spectator, and server interfaces output information as a log, which can be redirected to stderr.


### Spectator interface
- same as player interface except no keypress options (other than Q to quit)
- can see the whole map

## Data structures (e.g., struct names and members)
- `game struct`
- client
- map, which is an array
- array of clients
- FILE's
- `xy_coord` with an int y and int x

## Security and privacy properties
- always change the host name


## Error handling and recovery
- probably just exit the game when there is an error and send error message
- print statements

## Resource management
- make sure there are no memory leaks using valgrind
- make sure `endGame()` correctly frees all data stored

## Persistant storage (files, database, etc)
- master map - array of characters
- array of clients
