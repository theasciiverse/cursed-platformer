#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "level.h"

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

#define WALKING 0
#define JUMPING 1

struct entity {
    double x, y;
    double x_vel;
    double y_vel;
    char state;
} player;

struct Level* level;

void process(double delta, chtype input);

int main(int argc, char **argv) {
    level = load("level1.txt");
    
    initscr();
    resize_term(31, 80);
    nodelay(stdscr, TRUE);
    noecho();
    cbreak(); 
    keypad(stdscr, TRUE);
    curs_set(0);

    player.x = 2;
    player.y = 2;

    chtype input;
    
    clock_t last = clock();

    while (true) {
        input = getch();
        
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 80; x++) {
                move(y + 1, x);

                addch(level->data[y][x]); 
            }
        }
        
        move(player.y + 1, player.x);
        
        if (player.state == WALKING)
            addch('@');
        else
            addch('^');
        
        move(0,0);

        clock_t now = clock();
        double delta = ((double) (now - last) / CLOCKS_PER_SEC);
        
        printw("FPS: %f X: %f Y: %f X_vel: %f, Y_vel: %f", delta, player.x, player.y, player.x_vel, player.y_vel);

        refresh();
        
        now = clock();

        delta = ((double) (now - last) / CLOCKS_PER_SEC);
        
        last = now;

        process(delta, input);
    }

    endwin();

    return 0;
}

void process(double delta, chtype input) {
    switch(input) {
        case ERR:
            break;
        case KEY_SPACEBAR:
            if (player.state != JUMPING) {
                player.y_vel = -3;
                player.state = JUMPING;
            }
            break;
        case KEY_LEFT:
            player.x_vel = -10;
            break;
        case KEY_RIGHT:
            player.x_vel = 10;
            break;
    }
    
    int old_x = (int) player.x;
    int old_y = (int) player.y;

    player.x += player.x_vel * delta;
    player.y += player.y_vel * delta;

    int new_x = (int) player.x;
    if (level->data[old_y][new_x] == '#') {
        player.x = (float) old_x;
        player.x_vel = 0.0;
        new_x = old_x;
    }
    
    int new_y = (int) player.y;
    if (level->data[new_y][new_x] == '#') {
        player.y = (float) old_y;
        
        // Friction? Totes a bug but does what we want.
        player.x_vel = 0.0;
        player.state = WALKING;
        new_y = old_y;
    }

    if (level->data[new_y + 1][new_x] != '#') {
        player.y_vel += 4 * delta;
    }
}
