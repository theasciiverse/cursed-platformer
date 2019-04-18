#include <stdio.h>
#include <unistd.h>

#include "level.h"
#include "map.h"
#include "helper.h"

void processPhysics(struct Entity* entity, double delta) {
    int old_x = (int) entity->x;
    int old_y = (int) entity->y;

    struct Map* map = entity->level->map;

    if (entity->x_vel < 0 && !isPassable(map->data[old_y][old_x - 1])) {
        entity->x = (float) old_x;
        entity->x_vel = 0;
    }

    if (entity->x_vel > 0 && !isPassable(map->data[old_y][old_x + 1])) {
        entity->x = (float) old_x;
        entity->x_vel = 0;
    }

    entity->y += entity->y_vel * delta;
    entity->x += entity->x_vel * delta;

    int new_x = (int) entity->x;

    if (!isPassable(map->data[old_y][new_x])) {
        entity->x = (float) old_x;
        new_x = old_x;
    }

    int new_y = (int) entity->y;

    if (!isPassable(map->data[new_y][new_x])) {
        entity->y = (float) old_y;
        new_y = old_y;
    }

    if (isPassable(map->data[new_y + 1][new_x])) {
        entity->y_vel += 4 * delta;
    }

    char below = map->data[new_y + 1][new_x];
    if (below == '^') {
        entity->state = DEAD;
        entity->x = (float) new_x;
        entity->y = (float) new_y;
    } else if ((!isPassable(below) || below == '"' || below == 'T') && entity->y_vel >= 0) {
        entity->state = WALKING;
        entity->y_vel = 0;
        entity->y = (float) new_y;
    } else {
        entity->y_vel += 12 * delta;
        entity->state = AIRBORNE;
    }
}

struct Entity* createEntity() {
    struct Entity* entity = malloc(sizeof(struct Entity));

    memset(entity, 0, sizeof(struct Entity));

    return entity;
}

void destroyEntity(struct Entity* entity) {
    free(entity->internal_data);
    free(entity);
}

void processMessage(struct Entity* message, double delta);

struct Entity* createMessage(struct Level* level, double x, double y, char* who, char* msg, int read) {
    struct Entity* message = createEntity();

    message->x = x;
    message->y = y;
    message->width = 1;
    message->x_vel = 0;
    message->sprite = '?';
    message->state = IDLE;
    message->disabled = false;
    message->hidden = false;
    message->internal_data = malloc(sizeof(struct Message));

    struct Message* message_data = (struct Message*) message->internal_data;

    message->level = level;
    strcpy(message_data->who, who);
    strcpy(message_data->msg, msg);
    message_data->read = read;

    message->processFunc = &processMessage;
    return message;
}

void processMessage(struct Entity* message, double delta) {
    int x = (int) message->x;
    int y = (int) message->y;

    int px = (int) message->level->player->x;
    int py = (int) message->level->player->y;

    struct Message* message_data = (struct Message*) message->internal_data;

    if (!message_data->read && x == px && y == py) {
        message->level->player->x_vel = 0;
        message->level->player->y_vel = 0;
        showMessage(message_data->who, message_data->msg);
        message_data->read = true;
        message->disabled = true;
        message->sprite = '!';
    }
}

void processButton(struct Entity* message, double delta);

struct Entity* createButton(struct Level* level, double x, double y, double tx, double ty) {
    struct Entity* button = createEntity();

    button->x = x;
    button->y = y;
    button->width = 1;
    button->x_vel = 0;
    button->sprite = '_';
    button->state = IDLE;
    button->disabled = false;
    button->hidden = false;
    button->internal_data = malloc(sizeof(struct Button));

    struct Button* button_data = (struct Button*) button->internal_data;

    button->level = level;
    button_data->tx = tx;
    button_data->ty = ty;

    button->processFunc = &processButton;
    return button;
}

void processButton(struct Entity* button, double delta) {
    int x = (int) button->x;
    int y = (int) button->y;

    int px = (int) button->level->player->x;
    int py = (int) button->level->player->y;

    struct Button* button_data = (struct Button*) button->internal_data;

    if (x == px && y == py) {
        int tx = button_data->tx;
        int ty = button_data->ty;

        button->level->map->data[ty][tx] = ' ';
        button->disabled = true;
    }
}

void processTeleport(struct Entity* message, double delta);

struct Entity* createTeleport(struct Level* level, double x, double y, double tx, double ty) {
    struct Entity* teleport = createEntity();

    teleport->x = x;
    teleport->y = y;
    teleport->width = 1;
    teleport->x_vel = 0;
    teleport->sprite = '|';
    teleport->state = IDLE;
    teleport->disabled = false;
    teleport->hidden = false;
    teleport->internal_data = malloc(sizeof(struct Teleport));

    struct Teleport* teleport_data = (struct Teleport*) teleport->internal_data;

    teleport->level = level;
    teleport_data->tx = tx;
    teleport_data->ty = ty;

    teleport->processFunc = &processTeleport;
    return teleport;
}

void processTeleport(struct Entity* teleport, double delta) {
    int x = (int) teleport->x;
    int y = (int) teleport->y;

    int px = (int) teleport->level->player->x;
    int py = (int) teleport->level->player->y;

    struct Teleport* teleport_data = (struct Teleport*) teleport->internal_data;

    if (x == px && y == py) {
        int tx = teleport_data->tx;
        int ty = teleport_data->ty;

        teleport->level->player->x = tx;
        teleport->level->player->y = ty;
    }
}

void processPlatform(struct Entity* platform, double delta);

struct Entity* createPlatform(struct Level* level, int sx, int sy, int tx, int ty, int width) {
    struct Entity* platform = createEntity();

    platform->x = sx;
    platform->y = sy;
    platform->width = width;

    platform->x_vel = 0;
    platform->y_vel = 0;
    platform->sprite = 'T';
    platform->state = IDLE;
    platform->disabled = false;
    platform->hidden = false;
    platform->internal_data = malloc(sizeof(struct Platform));

    struct Platform* platform_data = (struct Platform*) platform->internal_data;

    platform->level = level;
    platform_data->sx = (double) sx;
    platform_data->sy = (double) sy;
    platform_data->tx = (double) tx;
    platform_data->ty = (double) ty;

    if (platform_data->sx == platform_data->tx) {
        platform_data->dir = 'V';
    } else {
        platform_data->dir = 'H';
    }

    platform->processFunc = &processPlatform;
    return platform;
}

void processPlatform(struct Entity* platform, double delta) {
    int x = (int) platform->x;
    int y = (int) platform->y;

    int px = (int) platform->level->player->x;
    int py = (int) platform->level->player->y;

    struct Platform* platform_data = (struct Platform*) platform->internal_data;

    platform->y_vel = 0;
    platform->x_vel = 0;

    if (platform_data->dir == 'V') {
        int dir = platform_data->ty - y;

        if (dir < 0) {
            platform->y_vel = -3;
        } else {
            platform->y_vel = 3;
        }
    } else {
        int dir = platform_data->tx - x;

        if (dir < 0) {
            platform->x_vel = -3;
        } else {
            platform->x_vel = 3;
        }
    }

    if (x == platform_data->tx && y == platform_data->ty) {
        platform_data->tx = platform_data->sx;
        platform_data->ty = platform_data->sy;

        platform_data->sx = x;
        platform_data->sy = y;

        platform->x = platform_data->sx;
        platform->y = platform_data->sy;
    }

    platform->x += platform->x_vel * delta;
    platform->y += platform->y_vel * delta;

    if (px >= x && px < x + platform->width && py == y - 1) {
        platform->level->player->state = WALKING;
        platform->level->player->y_vel = 0;
        platform->level->player->y = platform->y - 1;
        platform->level->player->x += platform->x_vel * delta;
    }
}

void processTrampoline(struct Entity* trampoline, double delta);

struct Entity* createTrampoline(struct Level* level, double x, double y, int width) {
    struct Entity* trampoline = createEntity();

    trampoline->x = x;
    trampoline->y = y;
    trampoline->width = width;
    trampoline->x_vel = 0;
    trampoline->sprite = 'Z';
    trampoline->state = IDLE;
    trampoline->disabled = false;
    trampoline->hidden = false;
    trampoline->internal_data = malloc(sizeof(struct Teleport));

    trampoline->level = level;

    trampoline->processFunc = &processTrampoline;
    return trampoline;
}

void processTrampoline(struct Entity* trampoline, double delta) {
    int x = (int) trampoline->x;
    int y = (int) trampoline->y;

    int px = (int) trampoline->level->player->x;
    int py = (int) trampoline->level->player->y;

    if (px >= x && px < x + trampoline->width && py == y - 1) {
        if (trampoline->level->player->y_vel > 4)
            trampoline->level->player->y_vel *= -0.9;
    }
}

void processContinue(struct Entity* trampoline, double delta);

struct Entity* createContinue(struct Level* level, double x, double y) {
    struct Entity* cont = createEntity();

    cont->x = x;
    cont->y = y;
    cont->width = 1;
    cont->x_vel = 0;
    cont->sprite = 'Z';
    cont->state = IDLE;
    cont->disabled = false;
    cont->hidden = true;
    cont->internal_data = malloc(sizeof(struct Teleport));

    cont->level = level;

    cont->processFunc = &processContinue;
    return cont;
}

void processContinue(struct Entity* cont, double delta) {
    int x = (int) cont->x;
    int y = (int) cont->y;

    int px = (int) cont->level->player->x;
    int py = (int) cont->level->player->y;

    if (px == x && py == y ) {
        if (access("autosave.sav", F_OK) != -1) {
            FILE* fp = fopen("autosave.sav", "r");
            char level_name[80];
            fscanf(fp, "%s", level_name);
            fclose(fp);

            switchLevel(&cont->level, level_name, cont->level->player); 
        }
    }
}
