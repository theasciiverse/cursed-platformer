#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "level.h"

struct Map {
    int width, height;
    char** data;
};

struct Map* loadMap(struct Metadata* metadata, char* name);

void destroyMap(struct Map* map);

#endif
