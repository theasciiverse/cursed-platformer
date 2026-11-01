#include <stdio.h>
#include <unistd.h>

#include "level.h"
#include "map.h"
#include "helper.h"
#include "sound.h"

void processPhysics(struct Entity* entity, double delta) {
    int old_x = (int) entity->x;
    int old_y = (int) entity->y;

    struct Map* map = entity->level->map;

    if (entity->x_vel < 0 && !isPassable(map->data[old_y][old_x - 1])) {
        entity->x = (float) old_x;
    }

    if (entity->x_vel > 0 && !isPassable(map->data[old_y][old_x + 1])) {
        entity->x = (float) old_x;
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
        if (entity->sprite != 'V') {
            playSound(SND_DEATH);
        }
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
    free(entity);
}

enum PF_return_type processMessage(struct Entity* message, double delta);

struct Entity* createMessage(struct Level* level, double x, double y, char* who, char* msg, int read) {
    createObjOf(Message, message, t_message);

    BaseOf(message).x = x;
    BaseOf(message).y = y;
    BaseOf(message).width = 1;
    BaseOf(message).x_vel = 0;
    BaseOf(message).sprite = '?';
    BaseOf(message).color = 7;
    BaseOf(message).state = IDLE;
    BaseOf(message).disabled = false;
    BaseOf(message).hidden = false;

    BaseOf(message).level = level;
    strncpy(message->who, who, sizeof(message->who)-1);
    strncpy(message->msg, msg, sizeof(message->msg)-1);
    message->read = read;

    BaseOf(message).processFunc = &processMessage;
    return EntityCast(message);
}

enum PF_return_type processMessage(struct Entity* _message, double delta) {
    struct Message* message = (struct Message*) _message;
    int x = (int) BaseOf(message).x;
    int y = (int) BaseOf(message).y;

    int px = (int) BaseOf(message).level->player->x;
    int py = (int) BaseOf(message).level->player->y;

    struct Message* message_data = (struct Message*)message;

    if (!message_data->read && x == px && y == py) {
        if (showMessage(message_data->who, message_data->msg)) {
            BaseOf(message).level->player->x_vel = 0;
            BaseOf(message).level->player->y_vel = 0;
        }
        message_data->read = true;
        BaseOf(message).disabled = true;
        BaseOf(message).sprite = '!';
        return pauze_timing;
    }
    return nothing_to_report;
}

enum PF_return_type processButton(struct Entity* message, double delta);

struct Entity* createButton(struct Level* level, double x, double y, double tx, double ty) {
    createObjOf(Button, button, t_button);

    BaseOf(button).x = x;
    BaseOf(button).y = y;
    BaseOf(button).width = 1;
    BaseOf(button).x_vel = 0;
    BaseOf(button).sprite = '_';
    BaseOf(button).state = IDLE;
    BaseOf(button).disabled = false;
    BaseOf(button).hidden = false;

    BaseOf(button).level = level;
    button->tx = tx;
    button->ty = ty;

    BaseOf(button).processFunc = &processButton;
    return EntityCast(button);
}

enum PF_return_type processButton(struct Entity* _button, double delta) {
    struct Button* button = (struct Button*) _button;
    int x = (int) BaseOf(button).x;
    int y = (int) BaseOf(button).y;

    int px = (int) BaseOf(button).level->player->x;
    int py = (int) BaseOf(button).level->player->y;

    struct Button* button_data = (struct Button*) button;

    if (x == px && y == py) {
        playSound(SND_PRESSUREPLATE);
        int tx = button_data->tx;
        int ty = button_data->ty;

        BaseOf(button).level->map->data[ty][tx] = ' ';
        BaseOf(button).disabled = true;
    }
    return nothing_to_report;
}

enum PF_return_type processTeleport(struct Entity* teleport, double delta);

struct Entity* createTeleport(struct Level* level, double x, double y, double tx, double ty) {
    createObjOf(Teleport, teleport, t_teleport);

    BaseOf(teleport).x = x;
    BaseOf(teleport).y = y;
    BaseOf(teleport).width = 1;
    BaseOf(teleport).x_vel = 0;
    BaseOf(teleport).sprite = '|';
    BaseOf(teleport).state = IDLE;
    BaseOf(teleport).disabled = false;
    BaseOf(teleport).hidden = false;

    BaseOf(teleport).level = level;
    teleport->tx = tx;
    teleport->ty = ty;

    BaseOf(teleport).processFunc = &processTeleport;
    return EntityCast(teleport);
}

enum PF_return_type processTeleport(struct Entity* _teleport, double delta) {
    struct Teleport* teleport = (struct Teleport*) _teleport;
    int x = (int) BaseOf(teleport).x;
    int y = (int) BaseOf(teleport).y;

    int px = (int) BaseOf(teleport).level->player->x;
    int py = (int) BaseOf(teleport).level->player->y;

    struct Teleport* teleport_data = (struct Teleport*) teleport;

    if (x == px && y == py) {
        int tx = teleport_data->tx;
        int ty = teleport_data->ty;

        BaseOf(teleport).level->player->x = tx;
        BaseOf(teleport).level->player->y = ty;
    }
    return nothing_to_report;
}

enum PF_return_type processPlatform(struct Entity* platform, double delta);

struct Entity* createPlatform(struct Level* level, int sx, int sy, int tx, int ty, int width) {
    createObjOf(Platform, platform, t_platform);

    BaseOf(platform).x = sx;
    BaseOf(platform).y = sy;
    BaseOf(platform).width = width;

    BaseOf(platform).x_vel = 0;
    BaseOf(platform).y_vel = 0;
    BaseOf(platform).sprite = 'T';
    BaseOf(platform).state = IDLE;
    BaseOf(platform).disabled = false;
    BaseOf(platform).hidden = false;

    BaseOf(platform).level = level;
    platform->sx = (double) sx;
    platform->sy = (double) sy;
    platform->tx = (double) tx;
    platform->ty = (double) ty;

    if (platform->sx == platform->tx) {
        platform->dir = 'V';
    } else {
        platform->dir = 'H';
    }

    BaseOf(platform).processFunc = &processPlatform;
    return EntityCast(platform);
}

enum PF_return_type processPlatform(struct Entity* _platform, double delta) {
    struct Platform* platform = (struct Platform*) _platform;
    int x = (int) BaseOf(platform).x;
    int y = (int) BaseOf(platform).y;

    int px = (int) BaseOf(platform).level->player->x;
    int py = (int) BaseOf(platform).level->player->y;

    struct Platform* platform_data = (struct Platform*) platform;

    BaseOf(platform).y_vel = 0;
    BaseOf(platform).x_vel = 0;

    if (platform_data->dir == 'V') {
        int dir = platform_data->ty - y;

        if (dir < 0) {
            BaseOf(platform).y_vel = -3;
        } else {
            BaseOf(platform).y_vel = 3;
        }
    } else {
        int dir = platform_data->tx - x;

        if (dir < 0) {
            BaseOf(platform).x_vel = -3;
        } else {
            BaseOf(platform).x_vel = 3;
        }
    }

    if (x == platform_data->tx && y == platform_data->ty) {
        platform_data->tx = platform_data->sx;
        platform_data->ty = platform_data->sy;

        platform_data->sx = x;
        platform_data->sy = y;

        BaseOf(platform).x = platform_data->sx;
        BaseOf(platform).y = platform_data->sy;
    }

    BaseOf(platform).x += BaseOf(platform).x_vel * delta;
    BaseOf(platform).y += BaseOf(platform).y_vel * delta;

    if (px >= x && px < x + BaseOf(platform).width && py == y - 1) {
        if (BaseOf(platform).level->player->state != DEAD)
            BaseOf(platform).level->player->state = WALKING;
        BaseOf(platform).level->player->y_vel = 0;
        BaseOf(platform).level->player->y = BaseOf(platform).y - 1;
        BaseOf(platform).level->player->x += BaseOf(platform).x_vel * delta;
    }
    return nothing_to_report;
}

enum PF_return_type processTrampoline(struct Entity* trampoline, double delta);

struct Entity* createTrampoline(struct Level* level, double x, double y, int width) {
    createObjOf(Trampoline, trampoline, t_trampoline);

    BaseOf(trampoline).x = x;
    BaseOf(trampoline).y = y;
    BaseOf(trampoline).width = width;
    BaseOf(trampoline).x_vel = 0;
    BaseOf(trampoline).sprite = 'Z';
    BaseOf(trampoline).state = IDLE;
    BaseOf(trampoline).disabled = false;
    BaseOf(trampoline).hidden = false;

    BaseOf(trampoline).level = level;

    BaseOf(trampoline).processFunc = &processTrampoline;
    return EntityCast(trampoline);
}

enum PF_return_type processTrampoline(struct Entity* _trampoline, double delta) {
    struct Trampoline* trampoline = (struct Trampoline*) _trampoline;
    int x = (int) BaseOf(trampoline).x;
    double y = BaseOf(trampoline).y;

    int px = (int) BaseOf(trampoline).level->player->x;
    double py = BaseOf(trampoline).level->player->y;

    if (px >= x && px < x + BaseOf(trampoline).width && py > y - 0.5 && py < y) {
        if (BaseOf(trampoline).level->player->y_vel > 4)
            BaseOf(trampoline).level->player->y_vel *= -0.9;
        BaseOf(trampoline).sprite = 'z';
        float vel = BaseOf(trampoline).level->player->y_vel * -1;
        if (vel > 6) {
            setSoundVolume(SND_TRAMPOLINE, (int) (((vel - 6) / 6) * 128) );
            playSound(SND_TRAMPOLINE);
        }
    } else {
        BaseOf(trampoline).sprite = 'Z';
    }
    return nothing_to_report;
}

enum PF_return_type processContinue(struct Entity* trampoline, double delta);
struct Entity* createContinue(struct Level* level, double x, double y) {
    createObjOf(Continue, cont, t_cont);

    BaseOf(cont).x = x;
    BaseOf(cont).y = y;
    BaseOf(cont).width = 1;
    BaseOf(cont).x_vel = 0;
    BaseOf(cont).sprite = 'Z';
    BaseOf(cont).state = IDLE;
    BaseOf(cont).disabled = false;
    BaseOf(cont).hidden = true;
    BaseOf(cont).level = level;

    BaseOf(cont).processFunc = &processContinue;
    return EntityCast(cont);
}

enum PF_return_type processContinue(struct Entity* _cont, double delta) {
    struct Continue* cont = (struct Continue*) _cont;
    int x = (int) BaseOf(cont).x;
    int y = (int) BaseOf(cont).y;

    int px = (int) BaseOf(cont).level->player->x;
    int py = (int) BaseOf(cont).level->player->y;

    if (px == x && py == y ) {
        FILE* fp = fopen(savePath(), "r");
        if (fp != NULL) {
            char level_name[80];
            int ok = fscanf(fp, "%79s", level_name) == 1;
            fclose(fp);

            if (ok) {
                switchLevel(level_name, BaseOf(cont).level->player); 
                return the_level_changed;
            }
        }
    }
    return nothing_to_report;
}

enum PF_return_type processGoal(struct Entity* goal, double delta);

struct Entity* createGoal(struct Level* level, double x, double y, char* next_level) {
    createObjOf(Goal, goal, t_goal);

    BaseOf(goal).x = x;
    BaseOf(goal).y = y;
    BaseOf(goal).width = 1;
    BaseOf(goal).x_vel = 0;
    BaseOf(goal).sprite = 'Z';
    BaseOf(goal).state = IDLE;
    BaseOf(goal).disabled = false;
    BaseOf(goal).hidden = true;
    
    strcpy(goal->next_level, next_level);

    BaseOf(goal).level = level;

    BaseOf(goal).processFunc = &processGoal;
    return EntityCast(goal);
}

enum PF_return_type processGoal(struct Entity* _goal, double delta) {
    struct Goal* goal = (struct Goal*) _goal;
    int x = (int) BaseOf(goal).x;
    int y = (int) BaseOf(goal).y;

    int px = (int) BaseOf(goal).level->player->x;
    int py = (int) BaseOf(goal).level->player->y;

    if (px == x && py == y) {
        playSound(SND_GOAL);

        FILE* fp = fopen(savePath(), "w");
        if (fp != NULL) {
            fprintf(fp, "%s\n", goal->next_level);
            fclose(fp);
        }

        switchLevel(goal->next_level, BaseOf(goal).level->player); 
        return the_level_changed;
    }
    return nothing_to_report;
}
