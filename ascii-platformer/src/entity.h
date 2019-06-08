#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "map.h"
#include "level.h"

enum E_type { 
    t_base = 0,
    t_message = 'M',
    t_goal = 'G',
    t_button = 'B',
    t_teleport = 'T',
    t_platform = 'P',
    t_trampoline = 'Z',
    t_bat = '^', 
    t_spike = 'V',
    t_muncher = '<',
    t_cont = 'C',
    t_bridge = '#'
};

enum PF_return_type {
    nothing_to_report = 0,
    the_level_changed = 1,
    pauze_timing = 2
};

    

struct Entity {
    enum E_type type;
    struct Level* level;
    int width;
    char hidden;
    char disabled;
    char sprite;
    char state;
    int color;
    double x, y, x_vel, y_vel;
    /**
     * if return != 0 -> stop further processing of entities!!
     * triggered level changes cause issues otherwise)
     */
    enum PF_return_type (*processFunc)(struct Entity*, double delta);
};
#define BaseOf(A) (A->base)
#define BasePtr(A) (A->base)
#define EntityCast(A) ((struct Entity*)A)

#define createObjOf(TYPE, NAME, ID) struct TYPE* NAME = \
            malloc(sizeof(struct TYPE));  \
            memset(NAME, 0, sizeof(struct TYPE));  \
            BaseOf(NAME).type = ID; 

void processPhysics(struct Entity* entity, double delta);

struct Entity* createEntity();

void destroyEntity(struct Entity* entity);

struct Message {
    struct Entity base;
    char who[80];
    char msg[362];
    int read;
};

struct Entity* createMessage(struct Level* level, double x, double y, char* who, char* msg, int read);

struct Goal {
    struct Entity base;
    char next_level[250];
};

struct Entity* createGoal(struct Level* level, double x, double y, char* next_level);

struct Button {
    struct Entity base;
    int tx, ty;
};

struct Entity* createButton(struct Level* level, double x, double y, double tx, double ty);

struct Teleport {
    struct Entity base;
    int tx, ty;
};

struct Entity* createTeleport(struct Level* level, double x, double y, double tx, double ty);

struct Platform {
    struct Entity base;
    int sx, sy;
    int tx, ty;

    char dir;
};

struct Entity* createPlatform(struct Level* level, int sx, int sy, int tx, int ty, int width);

struct Trampoline {
    struct Entity base;
};
struct Entity* createTrampoline(struct Level* level, double x, double y, int width);

struct Continue {
    struct Entity base;
};
struct Entity* createContinue(struct Level* level, double x, double y);

struct Bridge {
    struct Entity base;
    int xdir; // Assuming this is normalised
    double timer_acc, timer_delta;
};
struct Entity* createBridge(struct Level* level, double x, double y, int xdir);

#endif
