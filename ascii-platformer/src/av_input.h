/* Asciiverse: keyboard input with key releases (av_input.c). Public domain. */
#ifndef AV_INPUT_H
#define AV_INPUT_H

void av_input_init(void);   /* after initscr */
void av_input_end(void);    /* before endwin */
int av_input_releases(void); /* whether releases are reported */
int av_getkey(void);        /* the next key pressed, or ERR */
int av_getrelease(void);    /* the next key let go, or ERR */

#endif
