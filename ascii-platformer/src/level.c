#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "enemy.h"
#include "map.h"
#include "level.h"

struct Metadata* readMetadata(struct Level* level, char* path) {
    FILE* fp = fopen(path, "r");
    
    char command[80];
    struct Metadata* metadata = malloc(sizeof(struct Metadata));

    while (!feof(fp)) {
        fscanf(fp, "%s", command); 

        if (strcmp(command, "title") == 0) {
            char* title = malloc(sizeof(char) * 50);

            fgets(title, 50, fp);

            metadata->title = title;
        } else if (strcmp(command, "size") == 0) {
            int width;
            int height;

            fscanf(fp, "%d %d", &width, &height);

            metadata->width = width;
            metadata->height = height;
        } else if (strcmp(command, "start") == 0) {
            int x;
            int y;
            
            fscanf(fp, "%d %d", &x, &y);

            metadata->start_x = x;
            metadata->start_y = y;
        } else if (strcmp(command, "goal") == 0) {
            int x;
            int y;
            char* next_level = malloc(sizeof(char) * 80);

            fscanf(fp, "%d %d %s", &x, &y, next_level);

            metadata->goal_x = x;
            metadata->goal_y = y;
            metadata->next_level = next_level;
        } else if (strcmp(command, "muncher") == 0) {
            int x;
            int y;

            fscanf(fp, "%d %d", &x, &y);
            
            struct Entity* muncher = createMuncher(level, x, y);
            addEntity(level, muncher);
        }
    }
    
    fclose(fp);

    return metadata;
}

struct Level* loadLevel(char* name, struct Entity* player) {
    struct Level* level = malloc(sizeof(struct Level));

    char path[80] = "data/";
    strcat(path, name);
    
    char metadata_path[80];
    strcpy(metadata_path, path);
    strcat(metadata_path, "/metadata.txt");
    
    char map_path[80];
    strcpy(map_path, path);
    strcat(map_path, "/map.txt");
    
    level->enemy_count = 0;

    struct Metadata* metadata = readMetadata(level, metadata_path);
    
    level->metadata = metadata;
    level->map = loadMap(metadata, map_path);
    level->player = player;
    level->player->level = level;

    player->x = level->metadata->start_x;
    player->y = level->metadata->start_y;

    return level;
}

void addEntity(struct Level* level, struct Entity* entity) {
    entity->level = level;
    level->enemies[level->enemy_count] = entity;
    
    level->enemy_count++;
}

void destroyLevel(struct Level* level) {
    free(level->metadata->title);
    free(level->map);
    free(level->metadata->next_level);
    free(level->metadata);

    for (int i = 0; i < level->enemy_count; i++) {
        free(level->enemies[i]);
    }

    free(level);
}

void switchLevel(struct Level* level, char* name, struct Entity* player) {
    destroyLevel(level);

    level = loadLevel(name, player);
}
