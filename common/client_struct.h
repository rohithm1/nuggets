#ifndef __CLIENT_STRUCT_H
#define __CLIENT_STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../support/message.h"

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

#endif
