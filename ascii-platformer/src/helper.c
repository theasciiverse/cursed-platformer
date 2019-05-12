#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifndef NOSDL
#include <SDL.h>
#else
#include <unistd.h>
#include <stddef.h>
#endif

#define KEY_SPACEBAR ' '

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

WINDOW* mainwin;

void initHelp(WINDOW* _mainwin) {
    mainwin = _mainwin;
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

int pauseForAction();
static inline int wordlen(const char *str);
int wrap(char *str, const int cols);

/**
 * Shows a modal message window that can be closed with any key.
 * char*    who     Optional name of quotee, or an empty string.
 * char*    text    Message text. Can contain several lines of max
 *                  54 chars delimited with "\n".
 * return   int     1 if player should stop moving
 **/
int showMessage(char *who, char *text) {
    int stop = 0;
    WINDOW *frame;
    WINDOW *win;
    char *token;
    int i = 0;
    char str[400];
    strncpy(str, text, 400);
    int maxy = getmaxy(mainwin);
    int maxx = getmaxx(mainwin);
    int wh = MIN((float)maxy * 0.8, 16);
    int ww = MIN((float)maxx * 0.8, 56);
    int sh, sw;

    frame = subwin(mainwin, wh, ww, (float) (maxy - wh) / 2, (float) (maxx - ww) / 2);
    werase(frame);

    sh = wh - 2;
    // Smaller horizontal margins if space is tight
    if (ww > 40) {
        sw = ww - 4;
    } else {
        sw = ww - 2;
    }
    win = derwin(frame, sh, sw, (wh - sh) / 2, (ww - sw) / 2);

    if (strlen(who) > 0) {
        wattron(win, COLOR_PAIR(7));
        mvwprintw(win, 0, 0, "%s says:", who);
        wattroff(win, COLOR_PAIR(7));
    }
    i = 2;

    mvwaddstr(win, sh - 1, 0, "[ ");
    wattron(win, COLOR_PAIR(3));
    waddstr(win, "(A)");
    wattroff(win, COLOR_PAIR(3));
    waddstr(win, " / Space ]");

    box(frame, 0, 0);

    int lines = wrap(str, sw - 1);

    token = strtok(str, "\n");
    while (token != NULL) {
        mvwaddstr(win, sh - 1, sw - 2, ">>");
        if (i > sh - 3) {
            wrefresh(mainwin);
            wrefresh(frame);
            wrefresh(win);
            if (pauseForAction()) {
                stop = 1;
            }
            for (int j = 2; j < i; j++) {
                wmove(win, j, 0);
                wclrtoeol(win);
            }
            i = 2;
        }
        mvwaddnstr(win, i, 0, token, sw);
        i += 1;
        token = strtok(NULL, "\n");
    }
    wmove(win, sh - 1, sw - 2);
    wclrtoeol(win);

    wrefresh(mainwin);
    wrefresh(frame);
    wrefresh(win);
    if (pauseForAction()) {
        stop = 1;
    }
    delwin(win);
    delwin(frame);
    return stop;
}

int pauseForAction() {
    int stop = 0;
#ifndef NOSDL
    SDL_Event ev;
#endif
    chtype input;

    while (true) {
#ifndef NOSDL
        SDL_PumpEvents();
        SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_CONTROLLERBUTTONUP);
        if (ev.type == SDL_KEYUP &&
            (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_RIGHT)) {
            stop = 1;
        }
        if (ev.type == SDL_CONTROLLERBUTTONUP &&
            (ev.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
             ev.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
            stop = 1;
        }
        if ((ev.type == SDL_CONTROLLERBUTTONDOWN && ev.cbutton.button == SDL_CONTROLLER_BUTTON_A) ||
            (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_SPACE)) {
            break;
        }
#endif
        input = getch();
        if (input == KEY_LEFT || input == KEY_RIGHT) stop = 1;
        if (input == KEY_SPACEBAR) break;
#ifndef NOSDL
        SDL_Delay((long) (1.0 / 60) * 1000);
#else
        usleep((long) (1.0 / 60) * 1000000);
#endif
    }
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

int wordlen(const char *str) {
   int i = 0;
   while (str[i] != ' ' && str[i] != 0 && str[i] != '\n') {
      ++i;
   }
   return(i);
}

int wrap(char *str, const int cols) {
    int i = 0;
    int length = 0;
    int lines = 0;

    while (str[i] != '\0') {
        if (str[i] == '\n') {
            length = 0;
        } else if (str[i] == '\\') {
            str[i] = '\n';
            length = 0;
        } else if (str[i] == ' ') {
            if (length + wordlen(&str[i + 1]) >= cols) {
                str[i] = '\n';
                length = 0;
                lines++;
            }
        }

        length++;
        i++;
    }
    return lines;
}
