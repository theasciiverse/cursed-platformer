#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "map.h"

struct Level {
    struct Map* map;
    struct Entity* player;
    struct Entity* enemies[100];
    int enemy_count;
};

struct Level* loadLevel(char* name, struct Entity* player);

void addEntity(struct Level* level, struct Entity* entity);

void destroyLevel(struct Level* level); 

void switchLevel(struct Level* level, char* name, struct Entity* player);

#endif
