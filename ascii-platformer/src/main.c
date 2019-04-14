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

WINDOW *infowin;
WINDOW *mainwin;

void process(double delta, chtype input);

int main(int argc, char **argv) {
    level = loadLevel("level1", &player);
    
    initscr();
    resize_term(31, 80);
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
    
    clock_t last = clock();

    while (true) {
        input = getch();
        
        struct Map* map = level->map;

        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                mvwaddch(mainwin, y, x, map->data[y][x]);
            }
        }
        
        if (player.state != DEAD)
            mvwaddch(mainwin, player.y, player.x, '@');
        else
            mvwaddch(mainwin, player.y, player.x, 'X');
        
        wrefresh(mainwin);

        clock_t now = clock();
        double delta = ((double) (now - last) / CLOCKS_PER_SEC);
        
        char* statestr = (player.state == WALKING) ? "WALKING" : (player.state == AIRBORNE) ? "AIRBORNE" : "DEAD";
        char* debugstr = (char *)malloc(46);
        snprintf(debugstr, 46, "FPS: %-3i X:%-8f Y:%-8f ST:%-8s", (int) (delta * 1000000), player.x, player.y, statestr);
        mvwaddnstr(infowin, 0, 36, debugstr, 46);
        free(debugstr);

        wrefresh(infowin);
        
        now = clock();

        delta = ((double) (now - last) / CLOCKS_PER_SEC);
        
        last = now;

        process(delta, input);
    }

    endwin();

    return 0;
}

void process(double delta, chtype input) {
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
        char name[80];
        strcpy(name, level->metadata->next_level);

        switchLevel(level, name, &player); 
    }
    
    processPhysics(&player, delta); 
}
