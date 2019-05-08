#include <stdlib.h>
#include <math.h>
#include <curses.h>

#include "level.h"
#include "map.h"
#include "helper.h"
#include "enemy.h"
#include "sound.h"


enum PF_return_type processMuncher(struct Entity* enemy, double delta);

struct Entity* createMuncher(struct Level* level, double x, double y) {
    createObjOf(Muncher, muncher, t_muncher)

    BaseOf(muncher).type = t_muncher;
    BaseOf(muncher).x = x;
    BaseOf(muncher).y = y;
    BaseOf(muncher).width = 1;
    BaseOf(muncher).x_vel = -4;
    BaseOf(muncher).sprite = '<';
    BaseOf(muncher).color = 2;
    BaseOf(muncher).state = WALKING;
    BaseOf(muncher).disabled = false;
    BaseOf(muncher).hidden = false;
    BaseOf(muncher).level = level;

    muncher->sprite_timer = 0.0;

    BaseOf(muncher).processFunc = &processMuncher;
    return EntityCast(muncher);
}

enum PF_return_type processMuncher(struct Entity* _muncher, double delta) {
    struct Muncher* muncher = (struct Muncher*) _muncher;

    muncher->sprite_timer += delta;

    if (muncher->sprite_timer > 0.5) {
        if (BaseOf(muncher).sprite == '-')
            BaseOf(muncher).sprite = '<';
        else
            BaseOf(muncher).sprite = '-';
        muncher->sprite_timer = 0.0;
    }

    if (BaseOf(muncher).x_vel < 0 && BaseOf(muncher).sprite != '-')
        BaseOf(muncher).sprite = '>';
    else if (BaseOf(muncher).sprite != '-')
        BaseOf(muncher).sprite = '<';

    int x = (int) BaseOf(muncher).x;
    int y = (int) BaseOf(muncher).y;

    if (BaseOf(muncher).level->player->state != DEAD) {
        int px = (int) BaseOf(muncher).level->player->x;
        int py = (int) BaseOf(muncher).level->player->y;

        // Kill player on touch
        if (x == px && y == py) {
            BaseOf(muncher).level->player->state = DEAD;
            playSound(SND_DEATH);
        }
    }

    struct Map* map = BaseOf(muncher).level->map;

    char block_ahead;

    if (BaseOf(muncher).x_vel < 0)
        block_ahead = map->data[y][x - 1];
    else
        block_ahead = map->data[y][x + 1];

    if (block_ahead == '#' || block_ahead == '^')
        BaseOf(muncher).x_vel *= -1;

     if (BaseOf(muncher).x_vel < 0)
        block_ahead = map->data[y + 1][x - 1];
    else
        block_ahead = map->data[y + 1][x + 1];

    if (block_ahead == ' ' || block_ahead == '^')
        BaseOf(muncher).x_vel *= -1;

    processPhysics(EntityCast(muncher), delta);
    return nothing_to_report;
}


enum PF_return_type processSpike(struct Entity* enemy, double delta);

struct Entity* createSpike(struct Level* level, double x, double y) {
    createObjOf(Spike ,spike, t_spike);

    BaseOf(spike).type = t_spike;
    BaseOf(spike).x = x;
    BaseOf(spike).y = y;
    BaseOf(spike).width = 1;
    BaseOf(spike).x_vel = 0;
    BaseOf(spike).y_vel = 0;
    BaseOf(spike).sprite = 'V';
    BaseOf(spike).color = 7;
    BaseOf(spike).state = IDLE;
    BaseOf(spike).disabled = false;
    BaseOf(spike).hidden = false;
    BaseOf(spike).level = level;

    BaseOf(spike).processFunc = &processSpike;
    return EntityCast(spike);
}

enum PF_return_type processSpike(struct Entity* _spike, double delta) {
    if (_spike->state == GARBAGE)
        return nothing_to_report;
    struct Spike* spike = (struct Spike*)_spike;
    int x = (int) BaseOf(spike).x;
    int y = (int) BaseOf(spike).y;

    int px = (int) BaseOf(spike).level->player->x;
    int py = (int) BaseOf(spike).level->player->y;

    if (BaseOf(spike).state == IDLE) {
        if (x == px && y < py) {
            while (y < py) {
                if (!isPassable(BaseOf(spike).level->map->data[y][x]))
                    break;
                y++;

                if (y == py)
                    BaseOf(spike).state = AIRBORNE;
            }
        }
    } else if (BaseOf(spike).state == AIRBORNE) {
        if (x == px && y == py - 1 && BaseOf(spike).level->player->state != DEAD) {
            BaseOf(spike).level->player->state = DEAD;
            playSound(SND_DEATH);
        }
        processPhysics(EntityCast(spike), delta);
    } else if (BaseOf(spike).state != GARBAGE) {
        playSound(SND_RUBBLE);
        BaseOf(spike).sprite = '*';
        BaseOf(spike).state = GARBAGE;
    }
    return nothing_to_report;
}

enum PF_return_type processBat(struct Entity* enemy, double delta);

struct Entity* createBat(struct Level* level, double x, double y) {
    createObjOf(Bat, bat, t_bat);

    BaseOf(bat).type = t_bat;
    BaseOf(bat).x = x;
    BaseOf(bat).y = y;
    BaseOf(bat).width = 1;
    BaseOf(bat).x_vel = -4;
    BaseOf(bat).sprite = '^';
    BaseOf(bat).color = 5;
    BaseOf(bat).state = WALKING;
    BaseOf(bat).disabled = false;
    BaseOf(bat).hidden = false;

    BaseOf(bat).level = level;

    bat->sprite_timer = 0.0;

    BaseOf(bat).processFunc = &processBat;
    return EntityCast(bat);
}

enum PF_return_type processBat(struct Entity* _bat, double delta) {
    struct Bat* bat = (struct Bat*) _bat;

    bat->sprite_timer += delta;

    if (bat->sprite_timer > 0.2) {
        if (BaseOf(bat).sprite == '^')
            BaseOf(bat).sprite = 'v';
        else
            BaseOf(bat).sprite = '^';
        bat->sprite_timer = 0.0;
    }

    int x = (int) BaseOf(bat).x;
    int y = (int) BaseOf(bat).y;

    int px = (int) BaseOf(bat).level->player->x;
    int py = (int) BaseOf(bat).level->player->y;

    // Kill player on touch
    if (x == px && y == py && BaseOf(bat).level->player->state != DEAD) {
        BaseOf(bat).level->player->state = DEAD;
        playSound(SND_DEATH);
    }

    int dx = abs(x - px);
    int dy = abs(y - py);

    double dist = sqrt(pow((double) dx, 2) + pow((double) dy, 2));

    if (dist < 5) {
        BaseOf(bat).x_vel = px - x;
        BaseOf(bat).y_vel = py - y;
    }

    struct Map* map = BaseOf(bat).level->map;

    char block_ahead;

    if (BaseOf(bat).x_vel < 0)
        block_ahead = map->data[y][x - 1];
    else
        block_ahead = map->data[y][x + 1];

    if (block_ahead == '#' || block_ahead == '^')
        BaseOf(bat).x_vel *= -1;

    processPhysics(EntityCast(bat), delta);
    BaseOf(bat).y_vel = 0.0;
    return nothing_to_report;
}
