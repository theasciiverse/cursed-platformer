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

void destroyEntity(struct Entity* entity);

struct Message {
    int x,y;
    char* who;
    char* msg;
    int read;
};

struct Entity* createMessage(struct Level* level, double x, double y, char* who, char* msg, int read); 

#endif
