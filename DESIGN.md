# Design Spec for the Nuggets Game

# Team JYRE: Joanna, Yash, Rohith and Emma.


## User interface (including command-line parameters and stdin/stdout/graphical interface)
All user interface is on the command line and output by the server

### Client interface

#### Player interface
To initialize the `player` interface, the command line must receive
`./player hostname port playername`

the player interface will allow constant user input while the user is a part of the game - see the requirements spec for complete details regarding what character corresponds to which command

According to the requirements spec,
- The top line will provide a game status
- If other brief information needs to be displayed, it will be on the right of the top line

#### Spectator interface
To initialize the `spectator` interface, the command line must receive
`./player hostname port`

The spectator is different than the player in that the spectator will not have a name.

The user interface will only have the amount of gold that is unclaimed

### Server interface

To initialize the `server` interface, the command line must receive
`./server map.txt [seed]`.

The `seed` is an optional argument that will tell the game where to generate the random number of gold piles (and subsequently their locations).


### For All
The server must be initialized before any player. If a `playername` is NOT provided, then that initializes a `spectator`.

The player, spectator, and server interfaces output information as a log, which can be redirected to stderr.


## Inputs and Outputs

### game.c : the game module
Before any movements from the players are made or any messages are read in:
- the game will be passed a .txt `FILE` which it will give to the `map` module to construct a 1 dimensional `map`.  The `game` should then be able to take in a random number and place the nuggets into spaces. At this point, the `game` would update the status of the number of nuggets left based on the number of nuggets calculated to be used in the game. The actual coding that goes into will be a part of the `map.c` module which can be used to manipulate `map`. Therefore, the construction of the `map`  will be passed to the `map.c` module wherein the module will read from the `FILE` given to it and update the array.

Based on messages parsed by the `server`, certain functions in the `game` will be called, for example:
- based on the movements of a player, the server will call certain methods in the `game`, which in turn, calls methods from the `map` module. For example, if a  `player` inputs 'h', the server will call a function like ```moveLeft()```, and pass in params such as the `player` which called it. For each specific keystroke the user can input, there will be a function in the `game` module that therefore updates the `masterMap` as well as the `player` struct (whether that be changing the map or score or position, etc.)
- in addition, since the `game` module will be updating the player, etc., it needs to be able to rewrite the client map. Therefore it will call a function that handles visibility from the `map` module and output an individualized `map` for the client.


### server.c : the server module
Before any movements from the players are made or any messages are read in:
- The server will need to take in a .txt `FILE`, through ASCII characters as outlined in the req spec. It will need to be able to take in an optional number, or `seed`, for the random generation of the `map`. Both of these inputs will be passed through to the game module to be handled.

Based on messages parsed by the  `server`, certain functions in the `game` will be called, for example:
- The server must be able to take in messages from the player and parse them into certain outputs for the `game` module. For example, if a user inputs in 'h', the `server` must be able to know that that character was inputted and therefore call the respective function within the `game` module.
- The server will also have to be able to send messages to the clients (`player`). See the requirements spec for a full detailed list of what exactly will be given to the client at each different juncture.

### map.c : the module that handles map functionality
- Must be able to take in a `FILE`, and make it into an array which is the `map`. This is like the constructor for the `map` module
- There will be 2 functional constructors. The first will be the one for the `master map`, and the other will instantiate every character in the outline of the file as empty. Visibility issues will be updated in the `grid` module since it will have the `master map`.
- Correspondingly, the module must be able to take in a `map` to delete (free data)

### Client

#### Player
Before any movements from the players are made or any messages are read in:
- host name port and player name as command line arguments
- The player outputs logs to the user when other people collect gold and its current score.
- the player also displays it's individualized map that it receives from the server.
- the player will send a message to the server with the information it has based on the command line (name, etc.).
- The player outputs logs to the user when other people collect gold and its current score.


#### Spectator
Before any movements from the players are made or any messages are read in:
- Only host name as command line argument
- server will take it in and make a client without an actual name for it

## Functional decomposition into modules
### game.c :
- instantiate a new `game` module instance - will need to call a function in the `map` module to place all the random gold places
- a variety of functions to handle for differing user keystrokes (to move)
- write the specific `map` for each player based on visibility, as created by the `map` module
  - keep track of how much gold is left/everyone's score
  - handle the difference between knowing and seeing by calling a method from `map` that deals with visibility
- update each players `map` by calling a function from the `map` module

### map.c:
- construct a map - the map will contain an array as well as the file its reading from (NULL if it is a client)
- read a file passed in and construct an array based on it.
  - update the `map` given an index and a `client`
  - update the position of the client if applicable
  - if there is no file then instantiate each to a default value
- check what is a given position on a `map`
- check for `visibility` and adjust the client `map` accordingly
- delete a map (free data)


### server.c
- relay messages to the `client`
- log messages
  - based on parsed messages, call specific functions within the `game` module


### client.c
- need to display the `map` to the user
- log the messages to the user
- send messages to the server based on the information garnered


## Dataflow through modules
- check all of the arguments first
- initialize `game` module and create `map` from the given map text file.
- as each `client` joins game, it gets sent it's individualized `map`.
- `client` sends updates to the `server`
- `server` figures out which `client` sent the message
- passes `client id` and its new location to the `game`
- the `game` updates the `map`, and calculates new individual maps for EACH `client`
- `server` takes those new `maps` from its `game` instantiation and sends those back to the `clients`.

## Pseudo code

### game
`gameNew()`: will call `map.c` to make a new map, and will require a random number to determine where gold piles will be placed, and a file to which `map.c` will read from to create the new map

for each function that depends on user keystroke, the corresponding grid functions will have to perform certain actions on the client `map`. In addition, for specific special situations, like when the user grabs a nugget, it will need to update the score of each `player` as well as update the `masterMap` and the client `map`.

`gameEnd()`: has to handle all freeing data structures, will call `deleteMap()` to get rid of the mastermap.

*at any point if there are 0 nuggets left, the grid should call gameOver()*

### map
`mapNew()`: creates a new `map` based on the file being read in, or the `client map` if no files are given as parameters

functions to check for what is at a certain position in the map. For example, there is a function `checkIfWall()` and another `checkIfGold()`, etc.

`updateMap()`: based on the `map` and index it is given, it makes checks for what is at the given index and moves the `clients` as appropriate. It sends back messages to the `game` module so other aspects of the game can be updated

`addGold()`: adds gold to a given location on a the `map`, if possible

`visibility()`: calculates visibility from a given location on the `map` and updates the `client map` based on what is seen and what is known


### server
1. has to be constantly listening for messages
  - if there is a message that is from an unknown user, a new `client` must be made and added to the array
    - additionally must output the appropriate log
2. parse the messages and call specific functions in the `game` module
3. after movements/keystrokes are made, the server has to call the functions to update the clients in `game` module to update each `client map`.
4. if there are 0 nuggets at any point,
  1. `gameEnd()` will have to be called.
  2. all the `clients` will have to be deleted/freed
  3. `server` would stop listening to messages
  4. send final messages back to each `client` (will contain a gameover message)
5. send an individual message to each `client` with an update for the game

### client
1. Check arguments
2. If passes arguments, send its information to the `server` and ask to join the `server`, could be rejected, then quit. If it is allowed in the game, it saves important information such as its current score, the number of nuggets left in the `game`, and its name.
3. listen for keystrokes from the user to send to `server`
4. listens for messages from the `server` such as a game over message, a gold message, or just a notification of movement (which would be include the new` map` for it to display)
5. outputs status logs for the user.


## Major data structures
- a struct `client` which holds
   - `char* id` (unique)
   - `char* name`
   - `xy_coord_t position`
   - `char* map` (unique)
   - `int score`
   - `bool active`

*the spectator will have the exact same structure as the client, but with just no name*

- a struct `xy_coord`
  - int x coordinate
  - int y coordinate

- the game struct holds
  - int nuggetsLeft
  - int pilesLeft
  - char* masterMap;

*our map is going to be an array of characters*

## Testing plan
- test the `map` module
  - make sure map is being created correctly
  - make sure map is being updated correctly
  - check that visibility is working (difference between see and know)
- test the `game` module
  - make sure it creates the game module correctly (random gold locations)
  - make sure adds players correctly
  - check that it is dealing with each movement correctly
- check that the server works with the client provided
- check that the client works with the server provided
- one Player
  - 0 and 1 spectator
- multiple Players
  - 0 and 1 Spectator
- try to have a new spectator take over the old spectator's spot
- client tries to connect to an invalid host
- client joins mid Game
- try to add more players than max players (server should reject)
- Try having seed and no seed
- invalid map
- try to have spectator put in input
