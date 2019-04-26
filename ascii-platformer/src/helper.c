#ifndef USE_PDCURSES
#include <curses.h>
#else 
#include <PDCurses/curses.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>

WINDOW* mainwin;

void initHelp(WINDOW* mainwin) {
    mainwin = mainwin;
}

WINDOW* getMainWin() {
    return mainwin;
}

static struct Level* level;

struct Level* getCurrentLevel() {
    return level;
}

void setCurrentLevel(struct Level* new_level) {
    level = new_level;
}

/**
 * Shows a modal message window that can be closed with any key.
 * char*    who     Optional name of quotee, or an empty string.
 * char*    text    Message text. Can contain several lines of max
 *                  54 chars delimited with "\n".
 * return   int     1 if player should stop moving
 **/
int showMessage(char *who, char *text) {
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

    int stop = 0;

    while (true) {
        SDL_PumpEvents();
        SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_CONTROLLERBUTTONDOWN);
        if (ev.type == SDL_KEYUP) {
            if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_RIGHT) {
                stop = 1;
            }
        }
        if (getch() != ERR || ev.type == SDL_CONTROLLERBUTTONDOWN || ev.type == SDL_KEYDOWN) {
            if ((ev.key.keysym.sym != SDLK_LEFT && ev.key.keysym.sym != SDLK_RIGHT) || stop) {
                break;
            }
        }
        SDL_Delay(33);
    }
    delwin(win);
    return stop;
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
