#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#include <curses.h>


#define WALKING 0
#define AIRBORNE 1
#define IDLE 2
#define DEAD 9
#define GARBAGE 127

int showMessage(char* who, char* text);

void initHelp(WINDOW* mainwin);

WINDOW* getMainWin();

struct Level* getCurrentLevel(); 
void setCurrentLevel(struct Level* new_level);

int isPassable(chtype tile);

#endif
