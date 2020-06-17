/*
* CS50 19S Nuggets Game (final project)--JYRE
*
* The player module encapsulates both players and spectators
* in the Nuggets Game. Reads in input from the keyboard using the
* ncurses library. Connects to a Server with a port name known by the User
*
* Can send and receive string-type messages to the server using the message
* library in the support folder.
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
#include "player.h"
#include "message.h"
#include "file.h"


/***************** local funtions ****************/
static void readGoldMessage(char* content);
static void readGridMessage(char* content);
static void initializeCurses();
static void doResize();

/***************** global functions **************/
void checkScreenSize();
bool readUserInput();
bool parseMessage(void *arg, const addr_t from, const char* message);
void printLog(char* message);
void updateServer(addr_t to, char* message);
void leaveGame();
void displayMap();


/***************** global variables *************/
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



/****************** main ********************/
int main(const int argc, const char *argv[])
{
    //Check arguments: must be in the form -- hostname, port, [playername]
    //if there is no player name, then this is a spectator.

    if (argc > 4 || argc<3) {
        fprintf(stderr,"Wrong number of arguments provided. Must be in the form ./player hostname, port, [playername]");
    }

    else {
        //malloc the space for the necessary global variables to connect to the server
        hostname = malloc(strlen(argv[1]) + 1);
        port = malloc(strlen(argv[2]) + 1);
        address = malloc(sizeof(addr_t));
        playername = malloc(MaxNameLength + 1);
        strcpy(hostname, argv[1]);
        strcpy(port, argv[2]);

        purse = malloc(10*sizeof(char));
        nugsLeft= malloc(10 * sizeof(char));
        //display message should be at most the width of the screen
        displayMessage = malloc(50* sizeof(char));

        extraNotification = malloc(50* sizeof(char));
        for (int i = 0; i < 50; i ++) {
            displayMessage[i] = ' ';
            extraNotification[i] = ' ';
        }
        //initialize the messaging system
        message_init(stderr);
        //set the address
        message_setAddr(hostname, port, address);

        //if there is no 3rd arguement, the client is a spectator
        if (argc == 3) {
            //send spectator message to the server
            message_send(*address, "SPECTATE");
            isSpectator = true;
        }

        else if (argc==4) {

            //save the playername
            //if the name is longer than 50 characters, only copy the first 50 chars
            if (strlen(argv[3])>MaxNameLength) {
                strncpy(playername, argv[3], MaxNameLength);
            }

            else {
                strcpy(playername, argv[3]);
            }
            //send a message to the server reading "PLAY [playername]"
            char* message = malloc(strlen("PLAY")+ 51);
            sprintf(message, "PLAY %s", playername);
            message_send(*address, message);
            free(message);
        }

        message_loop(NULL, 0, NULL, readUserInput, parseMessage);
    }
}

/******************* doResize *********/
// is called when the window is resized, makes sure that ncurses
//does not quit out

static void
doResize()
{
    //end the current window
    endwin();

    //start a new window
    initscr();
    //keep the print statements on the new window
    mvprintw(0, 0, "your screen is too small\n");
    mvprintw(1, 0, "please expand your screen\n");
    mvprintw(2, 0, "press enter when ready\n");
    //make sure to refresh
    refresh();
}

/*******************checkScreenSize*********/
/* see client.h for description */


void
checkScreenSize()
{

    initializeCurses();
    //get the dimensions of the screen
    getmaxyx(stdscr, screenHeight, screenWidth);
    bool loop = true;

    //if the screen is big enough, then everything is good
    if (screenWidth>=ncols && screenHeight>=nrows) {
        screenSizeOK = true;
    }
    //otherwise
    else {
      //print out the error message
        mvprintw(0, 0, "your screen is too small, please make it larger\n");
        mvprintw(1, 0, "press enter when ready\n");

        //initiate the loop
        while (loop) {

            //catch window change signal, call doResize
            signal(SIGWINCH, doResize);
            //get the dimensions of the screen
            getmaxyx(stdscr, screenHeight, screenWidth);
            char c = getch();
            //if the user has hit the enter key
            if (c == '\n') {
              //and the screen is big enough
              //getmaxyx(stdscr, screenHeight, screenWidth);
                if ( screenWidth>=ncols && screenHeight>=nrows) {

                  //then break the loop
                  loop = false;
                }
            }
        }
    }

}

/*******************readUserInput***********/
/* see client.h for description */

bool
readUserInput()
{
    //read in character from the keyboard
    char c = getch();
    char *m;

    if (isSpectator) {

        if (c == 'Q') {
            //the spectator can only hit exit the game
            m = "KEY Q";
            updateServer(*address, m);
        }
        else {
            printLog("spectator cannot send keystrokes except for Q\n");
        }
    }

    else {
        if (c == EOF) {
            m = "KEY Q";
            updateServer(*address, m);
        }

        else {
            m = "KEY ";
            size_t len = strlen(m);
            char *m2 = malloc(len + 1 + 1 ); /* one for extra char, one for trailing zero */
            strcpy(m2, m);
            m2[len] = c;
            m2[len + 1] = '\0';
            updateServer(*address, m2);
            free(m2);
        }
        //the extra user notification should go away after the user
        //presses a new key
        displayExtraNotification= false;
    }
    return false;
}

/*******************parseMessage***********/
/* see client.h for description */

bool
parseMessage(void *arg, const addr_t from, const char* message)
{
    char* content = malloc(strlen(message) + 1);
    sprintf(content, "%s", message);
    content[strlen(message)] = '\0';

    //does different things based on the contents message as denoted by the first word
    if (strncmp(message,"NO", strlen("NO"))==0) {
        //player rejected from game or the server did not understand client message
        if (strlen(message) == strlen("NO")) {
            printLog("too many players are already playing the nuggets game on this server\n");

            //end the loop to quit the game
            free(content);

            leaveGame();
            return true;

        }

        else {
            //display the error message
            sprintf(extraNotification, "unknown keystroke");
            extraNotification[strlen("unknown keystroke")] = '\0';
            displayExtraNotification = true;
            displayMap();
            //log the error message
            printLog("The server did not understand command\n");
        }
    }
    else if (strncmp(message,"OK", strlen("OK"))==0) {
      //tell the player that they were able to enter the game
        if (!isSpectator) {
            id = message[strlen(message)-1];
        }

        printLog("successfully entered game");
    }

    else if (strncmp(message,"QUIT", strlen("QUIT"))==0) {
        //quit the game, end the loop
        attroff(COLOR_PAIR(1)); // turn off the screen color

        endwin(); // turn off ncurses display

        free(content);

        leaveGame();
        return true;
    }

    else if (strncmp(message,"DISPLAY", strlen("DISPLAY"))==0) {
        //display the map
        strcpy(currMap, content + strlen("DISPLAY "));
        displayMap();
    }

    else if (strncmp(message,"GRID", strlen("GRID"))==0) {
      //dimensions of the grid sent from server
        readGridMessage(content);
        checkScreenSize();
        currMap = malloc(screenWidth*screenHeight);

        for (int i = 0; i < screenWidth*screenHeight; i ++) {
            currMap[i] = ' ';
        }

    }
    else if (strncmp(message,"GOLD", strlen("GOLD"))==0) {
      //gold message from the server
        readGoldMessage(content);
    }

    else if (strncmp(message,"GAMEOVER", strlen("GAMEOVER"))==0) {
      //game over message from server, followed by a summary of the game

        attroff(COLOR_PAIR(1)); // turn off the screen color

        endwin(); // turn off ncurses display
        //read in the game summary
        fprintf(stdout, "%s\n",content);
        //end the loop to quit the game
        free(content);
        leaveGame();
        return true;
    }

    free(content);
    //need to call refresh first or else it will turn of ncurses before displaying
    //the message to the user
    return false;
}


/******************* displayMap ***********/
/* see client.h for description */

void
displayMap()
{
    clear();
    mvprintw(0, 0, displayMessage);
    int j = strlen(displayMessage);
    if (displayExtraNotification) {
        for (int i = 0; i < 50 - strlen(extraNotification) + strlen(displayMessage); i ++ ) {
            move(0, strlen(displayMessage) + 1);
            addch(' ');
        }

        mvprintw(0, strlen(displayMessage), extraNotification);
        j = j + strlen(extraNotification);
    }
    while (j<ncols) {
        addch(' ');
        move(0, j);
        j++;
    }
    mvprintw(1, 0, currMap);
    //first print the first line as the display message saved in the global variables
    refresh();
}

/*********** readGridMessage**********/
static void
readGridMessage(char* content)
{
    //read the message as ncols and nrows and converts them in integers
    content = content + strlen("GRID ");
    char* cols;
    char* rows;
    //read until it hits a space
    int i = 0;

    while (!isspace(content[i])) {
        i++;
    }
    rows = malloc(i + 1);
    strncpy(rows, content, i);
    rows[i] = '\0';

    //update the string that you are looking at, read until another space
    int j = i+1;

    while (j <= strlen(content)&& !isspace(content[j])) {
        j++;
    }
    cols = malloc( j - i + 1);
    strncpy(cols, content+i+1, j-i);
    //save these string as ints in the global variables.
    ncols = atoi(cols);
    nrows = atoi(rows);

    free(cols);
    free(rows);
}

/************* readGoldMessage ********/
/*
* This function reads in the gold message from the server
* contains the number of nuggets just collected, the number of nuggets left
* and the number of nuggets remaining in the purse
* and saves the new display message
*
*/
static void
readGoldMessage(char* content)
{
    char* n;
    content = content + strlen("GOLD ");
    // copy in the number of nuggets just collected
    int i = 0;
    fprintf(stderr, "content : %s\n", content);

    while (!isspace(content[i])) {
        i++;
    }
    n = malloc((i+2)*sizeof(char));

    for (int a = 0; a< 10; a++) {
        purse[a] = ' ';
        nugsLeft[a] = ' ';

        if (a<i+2) {
            n[a] = ' ';
        }
    }

    strncpy(n, content, i);
    n[i+1]='\0';
    // copy in what your new score is.
    int j = i+1;

    while (!isspace(content[j])) {
        j++;
    }

    strncpy(purse, content + i + 1, j-i - 1);
    purse[j-i-1] = '\0';
    // copy in how many nuggets are remaining.
    int k = j+1;

    while (k < strlen(content) && !isspace(content[k])) {
       k++;
    }

    strncpy(nugsLeft, content + j + 1, k-j);
    nugsLeft[k-j+1] = '\0';

    int collected = atoi(n);
    //change the diplay message to let the user know what just occurred
    if (isSpectator) {
        sprintf(displayMessage, "Spectator: %s nuggets unclaimed.", nugsLeft);
    }
    else {

      if (collected>0) {
          sprintf(extraNotification, "GOLD received: %d", collected);
          extraNotification[strlen("GOLD received: ") + strlen(n)] = '\0';
          displayExtraNotification = true;
      }

      else {
          displayExtraNotification = false;
      }

      sprintf(displayMessage, "Player %c has %s nuggets (%s nuggets unclaimed). ", id, purse, nugsLeft);
      displayMessage[strlen("Player A has  nuggets ( nuggets unclaimed). ") + strlen(purse) + strlen(nugsLeft) - 1] = '\0';
    }
    free(n);

}



/****************** printLog ***********/
/* see client.h for description */

void
printLog(char* message)
{
    //logs messages to stderr
    fprintf(stderr, "%s\n", message);
}

/*******************updateServer***********/
/* see client.h for description */

void
updateServer(addr_t to, char* message)
{
    //simply sends the mesage to the given address (the server)
    message_send(to, message);
}

/**************initializeCurses *************/

/* initializes the ncurses library */
static void
initializeCurses()
{
    //set up ncurses
    initscr(); // initialize the screen

    cbreak();  // accept keystrokes immediately
    noecho();  // don't echo characters to the screen

    //change the color of the screen--useful to make sure that ncurses is working
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
}

/*******************leaveGame***********/
/* see client.h for description */
void
leaveGame()
{
    //free everything that was malloc'ed as a global variable
    if (ncols>0) {
        free(currMap);
    }

    free(hostname);
    free(port);
    free(address);

    if (!isSpectator) {
        free(playername);
    }

    free(displayMessage);
    free(extraNotification);
    free(purse);
    free(nugsLeft);
    message_done();
}
