#include <stdlib.h>
#include "level.h"

#define WALKING 0;
#define AIRBORNE 1;
#define DEAD 9;

struct Enemy {
    struct Level* level;
    char type;
    char sprite;
    char state;
    double x, y, x_vel, y_vel;
    void* internal_data;
    void (*processFunc)(struct Enemy* enemy, double delta);
};

struct Muncher {
    double sprite_timer;  
};

void processMuncher(struct Enemy* enemy, double delta);

struct Enemy* createMuncher(struct Level* level, double x, double y) {
    struct Enemy* muncher = malloc(sizeof(struct Enemy));

    muncher->x = x;
    muncher->y = y;
    muncher->sprite = '<';
    muncher->state = WALKING; 
    muncher->internal_data = malloc(sizeof(struct Muncher));
    
    struct Muncher* muncher_data = (struct Muncher*) muncher->internal_data;

    muncher_data->sprite_timer = 0.0;

    muncher->processFunc = &processMuncher;
}

void destroyMuncher(struct Enemy* muncher) {
    free(muncher->internal_data);
    free(muncher);
}

void processMuncher(struct Enemy* muncher, double delta) {
    struct Muncher* muncher_data = (struct Muncher*) muncher->internal_data;  

    muncher_data->sprite_timer += delta;

    if (muncher_data->sprite_timer > 1.0) {
        if (muncher->sprite == '-')
            muncher->sprite = '<';
        else
            muncher->sprite = '-';
        muncher_data->sprite_timer = 0.0;
    }
}

