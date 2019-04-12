#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "level.h";

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

struct Enemy* createMuncher(struct Level* level, double x, double y);
void destroyMuncher(struct Enemy* muncher);
void processMuncher(struct Enemy* enemy, double delta);

#endif
