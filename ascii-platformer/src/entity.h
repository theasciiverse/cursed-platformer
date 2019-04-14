#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "map.h"

struct Entity {
    struct Level* level;
    char type;
    char sprite;
    char state;
    double x, y, x_vel, y_vel;
    void* internal_data;
    void (*processFunc)(struct Entity* entity, double delta);
};


void processPhysics(struct Entity* entity, double delta);

#endif
