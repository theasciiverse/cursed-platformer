#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "entity.h"
#include "level.h"

struct Muncher {
    struct Entity base;
    double sprite_timer;  
};

struct Entity* createMuncher(struct Level* level, double x, double y);
//int processMuncher(struct Entity* muncher, double delta);

struct Spike {
    struct Entity base;
};
struct Entity* createSpike(struct Level* level, double x, double y);
//int processSpike(struct Entity* spike, double delta); 

struct Bat {
    struct Entity base;
    double sprite_timer;
};

struct Entity* createBat(struct Level* level, double x, double y);
//int processBat(struct Entity* enemy, double delta);

#endif
