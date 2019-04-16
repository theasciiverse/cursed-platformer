#include <stdlib.h>
#include <math.h>

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

struct Spike {

};

void processSpike(struct Entity* enemy, double delta);

struct Entity* createSpike(struct Level* level, double x, double y) {
    struct Entity* spike = malloc(sizeof(struct Entity));

    spike->x = x;
    spike->y = y;
    spike->x_vel = 0;
    spike->y_vel = 0;
    spike->sprite = 'V';
    spike->state = IDLE; 
    spike->disabled = false;
    spike->hidden = false;
    spike->internal_data = malloc(sizeof(struct Spike));
    
    spike->level = level;

    struct Spike* spike_data = (struct Spike*) spike->internal_data;


    spike->processFunc = &processSpike;
    return spike;
}

void processSpike(struct Entity* spike, double delta) {
    
    int x = (int) spike->x;
    int y = (int) spike->y;

    int px = (int) spike->level->player->x;
    int py = (int) spike->level->player->y;
    
    if (spike->state == IDLE) {
        if (x == px && y < py) {
            while (y < py) {
                if (!isPassable(spike->level->map->data[y][x]))
                    break;
                y++;

                if (y == py)
                    spike->state = AIRBORNE;
            }

        }
     } else if (spike->state == AIRBORNE) {
        if (x == px && y == py - 1)
            spike->level->player->state = DEAD;
        processPhysics(spike, delta);
     } else {
        spike->sprite = '*';
        spike->state = GARBAGE;
     }
}

struct Bat {
    double sprite_timer;  
};

void processBat(struct Entity* enemy, double delta);

struct Entity* createBat(struct Level* level, double x, double y) {
    struct Entity* muncher = malloc(sizeof(struct Entity));

    muncher->x = x;
    muncher->y = y;
    muncher->x_vel = -4;
    muncher->sprite = '^';
    muncher->state = WALKING; 
    muncher->disabled = false;
    muncher->hidden = false;
    muncher->internal_data = malloc(sizeof(struct Bat));
    
    muncher->level = level; 

    struct Bat* muncher_data = (struct Bat*) muncher->internal_data;

    muncher_data->sprite_timer = 0.0;

    muncher->processFunc = &processBat;
    return muncher;
}

void processBat(struct Entity* bat, double delta) {
    struct Bat* bat_data = (struct Bat*) bat->internal_data;  

    bat_data->sprite_timer += delta;

    if (bat_data->sprite_timer > 0.2) {
        if (bat->sprite == '^')
            bat->sprite = 'v';
        else
            bat->sprite = '^';
        bat_data->sprite_timer = 0.0;
    }
    
    int x = (int) bat->x;
    int y = (int) bat->y;

    int px = (int) bat->level->player->x;
    int py = (int) bat->level->player->y;
    
    // Kill player on touch
    if (x == px && y == py) {
        bat->level->player->state = DEAD;
    } 
    
    int dx = abs(x - px);
    int dy = abs(y - py);
    
    double dist = sqrt(pow((double) dx, 2) + pow((double) dy, 2));
    
    if (dist < 5) {
        bat->x_vel = px - x;
        bat->y_vel = py - y;
    }

    struct Map* map = bat->level->map;

    char block_ahead;

    if (bat->x_vel < 0)
        block_ahead = map->data[y][x - 1];
    else
        block_ahead = map->data[y][x + 1];
    
    if (block_ahead == '#' || block_ahead == '^')
        bat->x_vel *= -1;
    
    processPhysics(bat, delta);
    bat->y_vel = 0.0;
}
