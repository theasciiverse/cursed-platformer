#include <stdlib.h>
#include "level.h"
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
    muncher->internal_data = malloc(sizeof(struct Muncher));
    
    muncher->level = level;

    struct Muncher* muncher_data = (struct Muncher*) muncher->internal_data;

    muncher_data->sprite_timer = 0.0;

    muncher->processFunc = &processMuncher;
}

void destroyMuncher(struct Entity* muncher) {
    free(muncher->internal_data);
    free(muncher);
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

    char block_ahead;

    if (muncher->x_vel < 0)
        block_ahead = muncher->level->data[y][x - 1];
    else
        block_ahead = muncher->level->data[y][x + 1];
    
    if (block_ahead == '#' || block_ahead == '^')
        muncher->x_vel *= -1;
   
     if (muncher->x_vel < 0)
        block_ahead = muncher->level->data[y + 1][x - 1];
    else
        block_ahead = muncher->level->data[y + 1][x + 1];
    
    if (block_ahead == ' ' || block_ahead == '^')
        muncher->x_vel *= -1;

    processPhysics(muncher, delta);
}

