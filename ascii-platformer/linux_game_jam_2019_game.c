#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <level.h>

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

#define WALKING 0
#define AIRBORNE 1
#define DEAD 9

struct entity {
    float x, y;
    float x_vel;
    float y_vel;
    char state;
} player;

struct Level* level;

void process(float delta, chtype input);

int main(int argc, char **argv) {
    level = load("level1.txt");
    
    initscr();
    resize_term(30, 80);
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
        
        for (int y = 0; y < 30; y++) {
            for (int x = 0; x < 80; x++) {
                move(y, x);

                addch(level->data[y][x]); 
            }
        }
        
        move(player.y, player.x);
        
        if (player.state != DEAD)
            addch('@');
        else
            addch('X');
        
        move(0,0);

        clock_t now = clock();
        float delta = ((float) (now - last) / CLOCKS_PER_SEC);
        
        char* statestr = (player.state == WALKING) ? "WALKING" : (player.state == AIRBORNE) ? "AIRBORNE" : "DEAD";
        printw("FPS: %f X: %f Y: %f ST: %s", delta, player.x, player.y, statestr);

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
			if (level->data[old_y][old_x - 1] != '#' && level->data[old_y][old_x - 1] != '^') {
				player.x -= 1;
			}
            break;
        case KEY_RIGHT:
			if (level->data[old_y][old_x + 1] != '#' && level->data[old_y][old_x + 1] != '^') {
				player.x += 1;
			}
    }

    player.y += player.y_vel * delta;

    int new_x = (int) player.x;
    int new_y = (int) player.y;

    if (level->data[new_y][new_x] == '#' || level->data[new_y][new_x] == '^') {
        player.y = (float) old_y;
        new_y = old_y;
    }
 
	char below = level->data[new_y + 1][new_x];
	if (below == '#') {
		player.state = WALKING;
		player.y_vel = 0;
		player.y = (float) new_y;
	} else if (below == '^') {
		player.x = (float) old_x;
		player.state = DEAD;
		player.y_vel = 0;
	} else {
        player.y_vel += 12 * delta;
		player.state = AIRBORNE;
	}
}
