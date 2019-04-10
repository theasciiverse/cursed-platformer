#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

#define WALKING 0
#define JUMPING 1

bool level[30][80];

struct entity {
    float x, y;
    float x_vel;
    float y_vel;
    char state;
} player;

void process(float delta, chtype input);

void generate_level() {
    srand(time(NULL));
    
    int y = 29;

    for (int x = 0; x < 80; x++) {
        level[y][x] = true;
    }
}

int main(int argc, char **argv) {
    generate_level();

    initscr();
    resize_term(40, 80);
    nodelay(stdscr, TRUE);
    noecho();
    cbreak(); 
    keypad(stdscr, TRUE);
    curs_set(0);

    player.x = 1;
    player.y = 1;

    chtype input;
    
    clock_t last = clock();

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
        
        move(0,0);

        clock_t now = clock();
        float delta = ((float) (now - last) / CLOCKS_PER_SEC);
        
        printw("FPS: %f X: %f Y: %f", delta, player.x, player.y);

        refresh();
        
        now = clock();

        delta = ((float) (now - last) / CLOCKS_PER_SEC);
        
        last = now;

        process(delta, input);
    }

    endwin();

    return 0;
}

void process(float delta, chtype input) {
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
    }
    
    int old_x = (int) player.x;
    int old_y = (int) player.y;

    player.x += player.x_vel * delta;
    player.y += player.y_vel * delta;

    int new_x = (int) player.x;
    if (level[old_y][new_x]) {
        player.x = (float) old_x;
        player.x_vel = 0.0;
        new_x = old_x;
    }

    int new_y = (int) player.y;
    if (level[new_y][new_x]) {
        player.y = (float) old_y;
        
        // Friction? Totes a bug but does what we want.
        player.x_vel = 0.0;
        player.state = WALKING;
        new_y = old_y;
    }

    if (!level[new_y + 1][new_x]) {
        player.y_vel += 3 * delta;
    }
}
