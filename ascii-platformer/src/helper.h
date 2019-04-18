#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#include <curses.h>

#define WALKING 0
#define AIRBORNE 1
#define IDLE 2
#define DEAD 9
#define GARBAGE 127

void showMessage(char* who, char* text);

void initHelp(WINDOW* mainwin);

WINDOW* getMainWin();

int isPassable(chtype tile);

#endif
