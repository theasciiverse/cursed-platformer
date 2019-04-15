#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#include <curses.h>

#define WALKING 0
#define AIRBORNE 1
#define IDLE 2
#define DEAD 9
#define GARBAGE 127

void showMessage(WINDOW* parentwin, char* who, char* text);

#endif
