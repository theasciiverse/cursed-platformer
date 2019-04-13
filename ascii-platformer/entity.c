#include "level.h"
#include "helper.h"

struct Entity {
    struct Level* level;
    char type;
    char sprite;
    char state;
    double x, y, x_vel, y_vel;
    void* internal_data;
    void (*processFunc)(struct Entity* entity, double delta);
};

void processPhysics(struct Entity* entity, double delta) {
    int old_x = (int) entity->x;
	int old_y = (int) entity->y;

    if (entity->level->data[old_y][old_x - 1] == '#' && entity->level->data[old_y][old_x - 1] == '^') {
        entity->x_vel = 0;
    }
    
    if (entity->level->data[old_y][old_x + 1] == '#' && entity->level->data[old_y][old_x + 1] == '^') {
        entity->x_vel = 0;
    }

    entity->y += entity->y_vel * delta;
    entity->x += entity->x_vel * delta;

    int new_x = (int) entity->x;
    
    if (entity->level->data[old_y][new_x] == '#' || entity->level->data[old_y][new_x] == '^') {
        entity->x = (float) old_x;
        new_x = old_x;
    }

    int new_y = (int) entity->y;

    if (entity->level->data[new_y][new_x] == '#' || entity->level->data[new_y][new_x] == '^') {
        entity->y = (float) old_y;
        new_y = old_y;
    }

    if (entity->level->data[new_y + 1][new_x] != '#') {
        entity->y_vel += 4 * delta;
    }
 
	char below = entity->level->data[new_y + 1][new_x];
	if (below == '#') {
		entity->state = WALKING;
		entity->y_vel = 0;
		entity->y = (float) new_y;
	} else if (below == '^') {
		entity->x = (float) old_x;
		entity->state = DEAD;
		entity->y_vel = 0;
	} else {
        entity->y_vel += 12 * delta;
		entity->state = AIRBORNE;
	}
}
