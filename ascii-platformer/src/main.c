#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "level.h"
#include "map.h"
#include "entity.h"
#include "enemy.h"
#include "helper.h"

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

struct Entity player;

struct Entity* muncher;

struct Level* level;

char current_level[80];

WINDOW *infowin;
WINDOW *mainwin;

void process(double delta, chtype input);

int debug = 0;

int main(int argc, char **argv) {
    initHelp(mainwin);

    if (argc > 1)
        level = loadLevel(argv[1], &player);
    else
        level = loadLevel("level1", &player);

    strcpy(current_level, level->name);

    initscr();
    resize_term(25, 80);
    nodelay(stdscr, TRUE);
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    infowin = newwin(1, 80, 0, 0);
    werase(infowin);
    mainwin = newwin(24, 80, 1, 0);
    werase(mainwin);

    chtype input;

    clock_t now = 0;
    clock_t last = clock();
    time_t sec;
    time(&sec);
    time_t lastsec = sec;
    int fps = 0;
    int frames = 0;

    double delta = 0;
    double slack = 0;

    char* debugstr = (char *)malloc(44);

    while (true) {
        input = getch();

        struct Map* map = level->map;

        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                mvwaddch(mainwin, y, x, map->data[y][x]);
            }
        }

        for (int i = 0; i < MAX_ENTITIES; i++) {
            if (level->enemies[i] != NULL) {
                if (!level->enemies[i]->disabled && !level->enemies[i]->hidden) {
                    for (int j = 0; j < level->enemies[i]->width; j++) {
                        mvwaddch(mainwin, level->enemies[i]->y, level->enemies[i]->x + j, level->enemies[i]->sprite);
                    }
                }
            }
        }

        if (player.state != DEAD)
            mvwaddch(mainwin, player.y, player.x, '@');
        else
            mvwaddch(mainwin, player.y, player.x, 'X');

        time(&sec);

        if (sec - lastsec > 0) {
            fps = frames;
            frames = 0;
            lastsec = sec;
        }
        frames++;

        now = clock();

        delta = ((double) (now - last) / CLOCKS_PER_SEC);
        slack = (1.0 / 60) - delta;
        if (slack > 0) {
            struct timespec delay;
            delay.tv_sec = 0;
            delay.tv_nsec = slack * 1000000000L;
            clock_nanosleep(CLOCK_REALTIME, 0, &delay, NULL);
            delta += slack;
        }

        last = now;

        mvwaddstr(infowin, 0, 0, "LEVEL ");
        mvwaddnstr(infowin, 0, 6, level->metadata->title, 70);
        if (debug) {
            char* statestr = (player.state == WALKING) ? "WALKING" : (player.state == AIRBORNE) ? "AIRBORNE" : "DEAD";
            snprintf(debugstr, 44, "FPS:%-3d X:%-9f Y:%-9f ST:%-8s", fps, player.x, player.y, statestr);
            mvwaddnstr(infowin, 0, 36, debugstr, 44);
        }
        wrefresh(infowin);

        for (int i = 0; i < MAX_ENTITIES; i++) {
            if (level->enemies[i] != NULL)
                if (!level->enemies[i]->disabled) {
                    level->enemies[i]->processFunc(level->enemies[i], delta);
                }
        }

        process(delta, input);
        wrefresh(mainwin);
    }

    free(debugstr);
    endwin();

    return 0;
}

void process(double delta, chtype input) {
    switch(input) {
        case ERR:
            break;
        case 'r':
            switchLevel(&level, current_level, &player);
            werase(mainwin);
            player.state = WALKING;
            break;
        case KEY_F(8):
            debug = !debug;
            werase(infowin);
    }

    if (player.state == DEAD)
        return;

    switch(input) {
        case ERR:
            break;
        case KEY_SPACEBAR:
            if (player.state == WALKING) {
                player.y_vel = -8;
                player.state = AIRBORNE;
            }
            break;
        case KEY_LEFT:
            if (player.x_vel < 0)
                player.x_vel = 0;
            else
                player.x_vel = -8;
            break;
        case KEY_RIGHT:
            if (player.x_vel > 0)
                player.x_vel = 0;
            else
                player.x_vel = 8;
    }

    int x = (int) player.x;
    int y = (int) player.y;

    if (x == level->metadata->goal_x && y == level->metadata->goal_y) {
        strcpy(current_level, level->metadata->next_level);
        switchLevel(&level, current_level, &player);
    }

    processPhysics(&player, delta);
}
