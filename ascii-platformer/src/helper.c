#include <curses.h>
#include <stdlib.h>
#include <string.h>

WINDOW* mainwin;

void initHelp(WINDOW* mainwin) {
    mainwin = mainwin;
}

WINDOW* getMainWin() {
    return mainwin;
}

/**
 * Shows a modal message window that can be closed with any key.
 * char* who    Optional name of quotee, or an empty string.
 * char* text   Message text. Can contain several lines of max 54 chars
 *              delimited with "\n".
 **/
void showMessage(WINDOW* parentwin, char *who, char *text) {
    WINDOW *win;
    char *token;
    int i = 1;
    char str[400];
    strncpy(str, text, 400);

    win = derwin(parentwin, 10, 60, 5, 10);
    werase(win);

    if (strlen(who) > 0) {
        mvwprintw(win, 1, 2, "%s says:", who);
        i += 2;
    }

    token = strtok(str, "\\");
    while (token != NULL) {
        mvwaddnstr(win, i, 2, token, 54);
        token = strtok(NULL, "\\");
        i++;
    }

    mvwprintw(win, 8, 2, "[...]");

    box(win, 0, 0);

    wgetch(win);
    delwin(win);
    return;
}

int isPassable(chtype tile) {
    switch (tile) {
        case '#':
            return false;
        case '^':
            return false;
        case ']':
            return false;
        case '[':
            return false;
        default:
            return true;
    }
}
