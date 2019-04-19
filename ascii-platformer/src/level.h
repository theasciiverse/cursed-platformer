#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "map.h"

#define MAX_ENTITIES 9001

struct Level {
    char name[80];
    struct Map* map;
    struct Metadata* metadata;
    struct Entity* player;
    struct Entity* enemies[MAX_ENTITIES];
    int enemy_count;
};

struct Metadata {
    char* title;

    int width, height;
    int start_x, start_y;
    int goal_x, goal_y;
};

struct Level* loadLevel(char* name, struct Entity* player);

void addEntity(struct Level* level, struct Entity* entity);

void destroyLevel(struct Level* level); 

void switchLevel(char* name, struct Entity* player);

#endif
