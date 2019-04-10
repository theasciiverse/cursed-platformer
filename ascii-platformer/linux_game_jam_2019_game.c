#include <stdio.h>
#include <curses.h>

#define PROJECT_NAME "linux-game-jam-2019-game"

int main(int argc, char **argv) {
    initscr();
    nodelay(stdscr, TRUE);
    
    char* msg = "Hello, World";

    while(true) {
        printw("%s", msg);
        refresh();
        char c = getch();

        if (c == 'y')
            msg = "Hello, you!";
        
        clear();
    }
    endwin();

    return 0;
}
