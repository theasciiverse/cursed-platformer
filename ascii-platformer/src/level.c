#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "map.h"

struct Level {
    struct Map* map;
    struct Entity* player;
    struct Entity* enemies[100];
    int enemy_count;
};

struct Level* loadLevel(char* name, struct Entity* player) {
    struct Level* level = malloc(sizeof(struct Level));

    char path[80] = "data/";
    strcat(path, name);
    
    level->map = loadMap(path);
    level->player = player;
    level->player->level = level;

    player->x = level->map->start_x;
    player->y = level->map->start_y;

    return level;
}

void addEntity(struct Level* level, struct Entity* entity) {
    entity->level = level;
    level->enemies[level->enemy_count] = entity;
    
    level->enemy_count++;
}

void destroyLevel(struct Level* level) {
    free(level->map);
    
    for (int i = 0; i < level->enemy_count; i++) {
        free(level->enemies[i]);
    }

    free(level);
}

void switchLevel(struct Level* level, char* name, struct Entity* player) {
    destroyLevel(level);

    level = loadLevel(name, player);
}
