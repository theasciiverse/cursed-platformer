#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "enemy.h"
#include "map.h"
#include "level.h"

#include "helper.h"

struct Metadata* readMetadata(struct Level* level, char* path) {
    FILE* fp = fopen(path, "r");

    char command[80];
    struct Metadata* metadata = malloc(sizeof(struct Metadata));

    while (!feof(fp)) {
        fscanf(fp, "%s", command);

        if (strcmp(command, "title") == 0) {
            char* title = malloc(sizeof(char) * 50);

            fseek(fp, 1, SEEK_CUR);
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
        } else if (strcmp(command, "spike") == 0) {
            int x;
            int y;

            fscanf(fp, "%d %d", &x, &y);

            struct Entity* spike = createSpike(level, x, y);
            addEntity(level, spike);
        } else if (strcmp(command, "bat") == 0) {
            int x;
            int y;

            fscanf(fp, "%d %d", &x, &y);

            struct Entity* bat = createBat(level, x, y);
            addEntity(level, bat);
        } else if (strcmp(command, "message") == 0) {
            int x, y;
            int readable;
            char* who = malloc(sizeof(char) * 40);
            char* msg = malloc(sizeof(char) * 250);

            fscanf(fp, "%d %d %d %s ", &x, &y, &readable, who);
            fgets(msg, 250, fp);

            struct Entity* message = createMessage(level, x, y, who, msg, readable);
            addEntity(level, message);
        } else if (strcmp(command, "button") == 0) {
            int x, y;
            int tx, ty;

            fscanf(fp, "%d %d %d %d", &x, &y, &tx, &ty);

            struct Entity* button = createButton(level, x, y, tx, ty);
            addEntity(level, button);
        } else if (strcmp(command, "teleport") == 0) {
            int x, y;
            int tx, ty;

            fscanf(fp, "%d %d %d %d", &x, &y, &tx, &ty);

            struct Entity* teleport = createTeleport(level, x, y, tx, ty);
            addEntity(level, teleport);
        } else if (strcmp(command, "platform") == 0) {
            int sx, sy;
            int tx, ty;
            int width;

            fscanf(fp, "%d %d %d %d %d", &sx, &sy, &tx, &ty, &width);

            struct Entity* platform = createPlatform(level, sx, sy, tx, ty, width);
            addEntity(level, platform);
        } else if (strcmp(command, "trampoline") == 0) {
            int sx, sy;

            fscanf(fp, "%d %d", &sx, &sy);

            struct Entity* trampoline = createTrampoline(level, sx, sy);
            addEntity(level, trampoline);
        }
    }

    fclose(fp);

    return metadata;
}

struct Level* loadLevel(char* name, struct Entity* player) {
    struct Level* level = malloc(sizeof(struct Level));
    level->name = name;

    char path[80] = "data/";
    strcat(path, name);

    char metadata_path[80];
    strcpy(metadata_path, path);
    strcat(metadata_path, "/metadata.txt");

    char map_path[80];
    strcpy(map_path, path);
    strcat(map_path, "/map.txt");

    level->enemy_count = 0;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        level->enemies[i] = NULL;
    }

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
    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (level->enemies[i] == NULL) {
            entity->level = level;
            level->enemies[i] = entity;
            level->enemy_count++;

            return;
        }

        if (level->enemies[i]->state == GARBAGE) {
            destroyEntity(level->enemies[i]);
            level->enemies[i] = entity;

            return;
        }
    }
}

void destroyLevel(struct Level* level) {
    free(level->metadata->title);
    free(level->map);
    free(level->metadata->next_level);
    free(level->metadata);

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (level->enemies[i] != NULL) {
            destroyEntity(level->enemies[i]);
        }
    }

    free(level);
}

void switchLevel(struct Level** level, char* name, struct Entity* player) {
    destroyLevel(*level);

    *level = loadLevel(name, player);
}
