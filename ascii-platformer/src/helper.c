#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

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
void showMessage(char *who, char *text) {
    SDL_Event ev;
    WINDOW *win;
    char *token;
    int i = 1;
    char str[400];
    strncpy(str, text, 400);

    win = derwin(mainwin, 12, 60, 5, 7);
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

    mvwprintw(win, 10, 2, "[...]");

    box(win, 0, 0);
    wrefresh(mainwin);
    wrefresh(win);

    while (true) {
        SDL_PollEvent(&ev);
        if (getch() != ERR || ev.type == SDL_CONTROLLERBUTTONDOWN)
            break;
        SDL_Delay(33);
    }
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
