# Testing Plan for the Nuggets Game

## Unit tests

### Map (the equivalent of grid)
1. check that a new map was being created and that it stored the map into an array correctly
2. test each of the functions that tests what is at a given location (example: check if a client is at a certain position, check if a certain position is empty, etc.)
3. check that the map was updated correctly when a player was moved into an empty spot on the map, and check that the correct letter is being returned
4. check that the map was updated correctly when a player was moved a spot with gold on the map, and check that the correct letter is being returned
5. check that the map was updated correctly when a player was moved a spot with another client on the map, and check that the correct letter is being returned
6. check that gold is being added when at a given position
7. check to make sure that a client cannot go "out of bounds" on the map


### Game
1. Make sure that a player can win the game and that everything quits properly
2. Test that player can leave the game at any time
3. Test that the player is denoted with an "@"
4. Test that the game will not withstand more than 26 `players` and 1 `spectator`
5. Test that the proper limited visibility is calculated for each individual client
6. test that a new player can join in the middle of the game.
7. Test two players running into each other
8. Test that a player can go into a spot that was once occupied by a player that left the game


### Server
1. Make sure that the server reads the inputted arguments and outputs a port for the user to connect to.
2. Allows up to 26 `players` to join the game
3. Allows 1 `spectator` to join the game\
4. Test that all messages are being sent in the proper order
5. Test with all types of maps--make sure that it works for all maps (that are valid)


### Client
1. Test with provided Server
2. test with our server
3. test with all different maps
4. Test with too small of a screen, ask for readjustments without having ncurses quit
5. Collect gold and make sure that the correct messages are being displayed
6. join as a spectator
7. try to move as spectator
8. quit out as a spectator and rejoin
9. enter invalid keystrokes and display and remove the appropriate message


## Integration tests

### Server side (with game and map)
using his player
one player
multiple players
spectator
different maps
visibility

### Client side
with his server
with our server


## System tests
connecting our own client and server
testing with different maps
testing with multiple clients
testing with spectator
testing when spectator is overriden by another spectator
