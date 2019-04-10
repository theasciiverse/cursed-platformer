#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

#define WALKING 0
#define JUMPING 1

bool level[80][40];

struct entity {
    int x, y;
    int x_vel;
    int y_vel;
    char state;
} player;

void process(chtype input);

void generate_level() {
    srand(time(NULL));

    for (int y = 0; y < 40; y++) {
        for (int x = 0; x < 80; x++) {
            if (rand() % 10 < 3)
                level[y][x] = true;
            else
                level[y][x] = false;
        }
    }
}

int main(int argc, char **argv) {
    generate_level();

    initscr();
    nodelay(stdscr, TRUE);
    noecho();
    cbreak(); 
    keypad(stdscr, TRUE);

    player.x = 1;
    player.y = 1;

    chtype input;

    while (true) {
        input = getch();
        
        for (int y = 0; y < 40; y++) {
            for (int x = 0; x < 80; x++) {
                move(y, x);

                if (level[y][x]) {
                    addch('#');
                } else {
                    addch(' ');
                }
            }
        }
        
        move(player.y, player.x);
        
        if (player.state == WALKING)
            addch('@');
        else
            addch('^');

        refresh();

        process(input);
        sleep(1);
    }

    endwin();

    return 0;
}

void process(chtype input) {
    switch(input) {
        case ERR:
            player.x_vel = 0;
            break;
        case KEY_SPACEBAR:
            if (player.state != JUMPING) {
                player.y_vel = -3;
                player.state = JUMPING;
            }
            break;
        case KEY_LEFT:
            player.x_vel = -1;
            break;
        case KEY_RIGHT:
            player.x_vel = 1;
    }

    if (level[player.y][player.x + player.x_vel]) {
        player.x_vel = 0;
    }
    
    player.x += player.x_vel;

    if (player.y_vel > 0 && level[player.y + 1][player.x]) {
        player.y_vel = 0;
        player.state = WALKING;
    } else if (player.y_vel < 0 && level[player.y - 1][player.x]) {
        player.y_vel = 0;
        player.state = WALKING;
    }
    
    if (!level[player.y + 1][player.x]) {
        player.y_vel++;
    }
    
    if (player.y_vel > 0)
        player.y += 1;
    else if (player.y_vel < 0)
        player.y -= 1;
}
