#include <stdlib.h>
#include "level.h"
#include "map.h"
#include "helper.h"
#include "entity.h"

struct Muncher {
    double sprite_timer;  
};

void processMuncher(struct Entity* enemy, double delta);

struct Entity* createMuncher(struct Level* level, double x, double y) {
    struct Entity* muncher = malloc(sizeof(struct Entity));

    muncher->x = x;
    muncher->y = y;
    muncher->x_vel = -4;
    muncher->sprite = '<';
    muncher->state = WALKING; 
    muncher->disabled = false;
    muncher->hidden = false;
    muncher->internal_data = malloc(sizeof(struct Muncher));
    
    muncher->level = level;

    struct Muncher* muncher_data = (struct Muncher*) muncher->internal_data;

    muncher_data->sprite_timer = 0.0;

    muncher->processFunc = &processMuncher;
    return muncher;
}

void processMuncher(struct Entity* muncher, double delta) {
    struct Muncher* muncher_data = (struct Muncher*) muncher->internal_data;  

    muncher_data->sprite_timer += delta;

    if (muncher_data->sprite_timer > 0.5) {
        if (muncher->sprite == '-')
            muncher->sprite = '<';
        else
            muncher->sprite = '-';
        muncher_data->sprite_timer = 0.0;
    }
    
    if (muncher->x_vel < 0 && muncher->sprite != '-')
        muncher->sprite = '>';
    else if (muncher->sprite != '-')
        muncher->sprite = '<';
    
    int x = (int) muncher->x;
    int y = (int) muncher->y;

    int px = (int) muncher->level->player->x;
    int py = (int) muncher->level->player->y;
    
    // Kill player on touch
    if (x == px && y == py) {
        muncher->level->player->state = DEAD;
    }

    struct Map* map = muncher->level->map;

    char block_ahead;

    if (muncher->x_vel < 0)
        block_ahead = map->data[y][x - 1];
    else
        block_ahead = map->data[y][x + 1];
    
    if (block_ahead == '#' || block_ahead == '^')
        muncher->x_vel *= -1;
   
     if (muncher->x_vel < 0)
        block_ahead = map->data[y + 1][x - 1];
    else
        block_ahead = map->data[y + 1][x + 1];
    
    if (block_ahead == ' ' || block_ahead == '^')
        muncher->x_vel *= -1;

    processPhysics(muncher, delta);
}

struct Stalaktite {

};

void processStalaktite(struct Entity* enemy, double delta);

struct Entity* createStalaktite(struct Level* level, double x, double y) {
    struct Entity* stalaktite = malloc(sizeof(struct Entity));

    stalaktite->x = x;
    stalaktite->y = y;
    stalaktite->x_vel = 0;
    stalaktite->sprite = 'V';
    stalaktite->state = IDLE; 
    stalaktite->disabled = false;
    stalaktite->hidden = false;
    stalaktite->internal_data = malloc(sizeof(struct Stalaktite));
    
    stalaktite->level = level;

    struct Stalaktite* stalaktite_data = (struct Stalaktite*) stalaktite->internal_data;


    stalaktite->processFunc = &processStalaktite;
    return stalaktite;
}

void processStalaktite(struct Entity* stalaktite, double delta) {
    
    int x = (int) stalaktite->x;
    int y = (int) stalaktite->y;

    int px = (int) stalaktite->level->player->x;
    int py = (int) stalaktite->level->player->y;
    
    if (stalaktite->state == IDLE) {
        if (x == px && y < py) {
            while (y < py) {
                if (!isPassable(stalaktite->level->map->data[y][x]))
                    break;
                y++;

                if (y == py)
                    stalaktite->state = AIRBORNE;
            }

        }
     } else if (stalaktite->state == AIRBORNE) {
        if (x == px && y == py - 1)
            stalaktite->level->player->state = DEAD;
        processPhysics(stalaktite, delta);
     } else {
        stalaktite->sprite = '*';
        stalaktite->state = GARBAGE;
     }
}
