#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "level.h"
#include "entity.h"
#include "enemy.h"
#include "helper.h"

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

struct Entity player;

struct Entity* muncher;

struct Level* level;


void process(double delta, chtype input);

int main(int argc, char **argv) {
    level = load_level("data/level1.txt");
    
    muncher = (struct Enemy*) createMuncher(level, 5.0, 5.0);


    initscr();
    resize_term(31, 80);
    nodelay(stdscr, TRUE);
    noecho();
    cbreak(); 
    keypad(stdscr, TRUE);
    curs_set(0);

    player.x = 2;
    player.y = 2;
    player.level = level;

    chtype input;
    
    clock_t last = clock();

    while (true) {
        input = getch();
        
        for (int y = 0; y < 30; y++) {
            for (int x = 0; x < 80; x++) {
                move(y + 1, x);

                addch(level->data[y][x]); 
            }
        }
        
        move(player.y + 1, player.x);
        
        if (player.state != DEAD)
            addch('@');
        else
            addch('X');
        
        move(muncher->y + 1, muncher->x);
        addch(muncher->sprite);

        move(0,0);

        clock_t now = clock();
        double delta = ((double) (now - last) / CLOCKS_PER_SEC);
        
        char* statestr = (player.state == WALKING) ? "WALKING" : (player.state == AIRBORNE) ? "AIRBORNE" : "DEAD";
        printw("FPS: %f X: %f Y: %f X_VEL: %f ST: %s     ", delta, player.x, player.y, player.x_vel, statestr);

        refresh();
        
        now = clock();

        delta = ((double) (now - last) / CLOCKS_PER_SEC);
        
        last = now;

        process(delta, input);

        processMuncher(muncher, delta);
    }

    endwin();

    return 0;
}

void process(double delta, chtype input) {
	if (player.state == DEAD)
		return;

	int old_x = (int) player.x;
	int old_y = (int) player.y;

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
    
    processPhysics(&player, delta); 
}
