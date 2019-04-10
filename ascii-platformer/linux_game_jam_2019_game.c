#include <stdio.h>
#include <curses.h>

#define PROJECT_NAME "linux-game-jam-2019-game"

char level[80][40];

WINDOW *create_new_win(int height, int width, int starty, int startx) {
    WINDOW* local;

    local = newwin(height, width, startx, starty);
    box(local, 0, 0);
    wrefresh(local);
    
    return local;
}

int main(int argc, char **argv) {
    initscr();
    nodelay(stdscr, TRUE);
    noecho();

    int x, y = 1;

    while (true) {
        char c = getch();

        switch(c) {
            case 'h':
                x--;
                break;
            case 'l':
                x++;
                break;
            case 'j':
                y++;
                break;
            case 'k':
                y--;
                break;
            case 'x':
                addch('x');
                break;
            case 'y':
                addch('y');
                break;
        }
        
        move(y, x);
        refresh();
    }

    endwin();

    return 0;
}
