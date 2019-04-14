#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "level.h";

struct Muncher {
    double sprite_timer;  
};

struct Enemy* createMuncher(struct Level* level, double x, double y);
void destroyMuncher(struct Entity* muncher);
void processMuncher(struct Entity* muncher, double delta);

#endif
