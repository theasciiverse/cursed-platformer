#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "level.h"

struct Muncher {
    double sprite_timer;  
};

struct Entity* createMuncher(struct Level* level, double x, double y);
void processMuncher(struct Entity* muncher, double delta);

struct Entity* createStalaktite(struct Level* level, double x, double y);
void processStalaktite(struct Entity* stalaktite, double delta); 

struct Bat {
};

struct Entity* createBat(struct Level* level, double x, double y);
void processBat(struct Entity* enemy, double delta);

#endif
