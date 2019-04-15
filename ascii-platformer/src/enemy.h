#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "level.h"

struct Muncher {
    double sprite_timer;  
};

struct Entity* createMuncher(struct Level* level, double x, double y);
void destroyMuncher(struct Entity* muncher);
void processMuncher(struct Entity* muncher, double delta);

struct Entity* createStalaktite(struct Level* level, double x, double y);
void destroyStalaktite(struct Entity* stalaktite);
void processStalaktite(struct Entity* stalaktite, double delta); 

#endif
