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

    char map_path[80];
    strcpy(map_path, path);
    strcat(map_path, "/map.txt");
    
    level->map = loadMap(map_path);
    level->player = player;
    
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
