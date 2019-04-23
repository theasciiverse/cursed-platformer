#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "enemy.h"
#include "map.h"
#include "level.h"

#include "helper.h"


void processMetaDataLine(char const * line, struct Metadata* metadata, struct Level* level);

struct Metadata* readMetadata(struct Level* level, char* path) {
    FILE* fp = fopen(path, "r");

    struct Metadata* metadata = malloc(sizeof(struct Metadata));

    while (!feof(fp)) {
        char  *command = NULL;
        size_t n = 0;
        getline(&command, &n, fp);
        if(n) {
            processMetaDataLine(command, metadata, level);
        }
        if(command) {
            free(command);
        }
    }

    fclose(fp);

    return metadata;
}

struct Level* loadLevel(char* name, struct Entity* player) {
    struct Level* level = malloc(sizeof(struct Level));
    
    memset(level, 0, sizeof(struct Level));

    strcpy(level->name, name);
    
    char path[80] = "data/";
    strcat(path, name);

    char metadata_path[80];
    strcpy(metadata_path, path);
    strcat(metadata_path, "/metadata.txt");

    level->enemy_count = 0;
    for (int i = 0; i < MAX_ENTITIES; i++) {
        level->enemies[i] = NULL;
    }

    struct Metadata* metadata = readMetadata(level, metadata_path);

    level->metadata = metadata;
    level->map = loadMap(metadata, path);
    level->player = player;
    level->player->level = level;

    player->x_vel = 0;
    player->y_vel = 0;

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
    free(level->metadata);
    destroyMap(level->map);

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (level->enemies[i] != NULL) {
            destroyEntity(level->enemies[i]);
        }
    }

    free(level);
}

void switchLevel(char* name, struct Entity* player) {
    char buf[80];
    strcpy(buf, name);
    
    struct Level* level = getCurrentLevel();

    destroyLevel(level);
    
    setCurrentLevel(loadLevel(buf, player));
}

void processMetaDataLine(char const * line, struct Metadata* metadata, struct Level *level) {
    char const *command = line;
    const int command_length = strlen(line);

        if (memcmp(command, "title", 5) == 0) {
            command += 6;
            strncpy(metadata->title, command, sizeof(metadata->title));
        } else if (memcmp(command, "size", 4) == 0) {
            int width;
            int height;
            command += 5;
            sscanf(command, "%d %d", &width, &height);
            metadata->width = width;
            metadata->height = height;
        } else if (memcmp(command, "start", 5) == 0) {
            int x;
            int y;
            command += 6;
            sscanf(command, "%d %d", &x, &y);
            metadata->start_x = x;
            metadata->start_y = y;
        } else if (memcmp(command, "goal", 4) == 0) {
            int x;
            int y;
            command += 5;
            char next_level[80];

            sscanf(command, "%d %d %s", &x, &y, next_level);

            struct Entity* goal = createGoal(level, x, y, next_level); 
            addEntity(level, goal);
        } else if (memcmp(command, "muncher", 7) == 0) {
            int x;
            int y;
            command += 8;

            sscanf(command, "%d %d", &x, &y);

            struct Entity* muncher = createMuncher(level, x, y);
            addEntity(level, muncher);
        } else if (memcmp(command, "spike", 5) == 0) {
            int x;
            int y;
            command += 6;
            sscanf(command, "%d %d", &x, &y);

            struct Entity* spike = createSpike(level, x, y);
            addEntity(level, spike);
        } else if (memcmp(command, "bat", 3) == 0) {
            int x;
            int y;
            command += 4;
            sscanf(command, "%d %d", &x, &y);

            struct Entity* bat = createBat(level, x, y);
            addEntity(level, bat);
        } else if (memcmp(command, "message", 7) == 0) {
            int x, y;
            int readable;
            char who[40];
            char msg[250];
            command += 8;

            int bytesread = sscanf(command, "%d %d %d %s %[^\n]\n", &x, &y, &readable, who, msg);

            struct Entity* message = createMessage(level, x, y, who, msg, readable);
            addEntity(level, message);
        } else if (memcmp(command, "button", 6) == 0) {
            int x, y;
            int tx, ty;
            command += 7;

            sscanf(command, "%d %d %d %d", &x, &y, &tx, &ty);

            struct Entity* button = createButton(level, x, y, tx, ty);
            addEntity(level, button);
        } else if (memcmp(command, "teleport", 8) == 0) {
            int x, y;
            int tx, ty;
            command += 9;
            sscanf(command, "%d %d %d %d", &x, &y, &tx, &ty);

            struct Entity* teleport = createTeleport(level, x, y, tx, ty);
            addEntity(level, teleport);
        } else if (memcmp(command, "platform", 8) == 0) {
            int sx, sy;
            int tx, ty;
            int width;
            command += 9;

            sscanf(command, "%d %d %d %d %d", &sx, &sy, &tx, &ty, &width);

            struct Entity* platform = createPlatform(level, sx, sy, tx, ty, width);
            addEntity(level, platform);
        } else if (memcmp(command, "trampoline", 10) == 0) {
            int sx, sy, width;
            command += 11;
            sscanf(command, "%d %d %d", &sx, &sy, &width);

            struct Entity* trampoline = createTrampoline(level, sx, sy, width);
            addEntity(level, trampoline);
        } else if (memcmp(command, "continue", 8) == 0) {
            int x, y;
            command += 9;

            sscanf(command, "%d %d", &x, &y);

            struct Entity* cont = createContinue(level, x, y);
            addEntity(level, cont);
        } else if (strcmp(command, "//") == 0) {
//            char buf[250];
//            fgets(buf, 250, fp);
        } 

}
