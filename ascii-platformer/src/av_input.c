/*
 * Asciiverse: keyboard input with key releases.
 *
 * A terminal sends characters, and repeats them while a key is held, but
 * never says when a key goes up. So upstream walks on one tap of an arrow
 * and stops on the next. Terminals that speak the kitty keyboard protocol
 * (kitty, Ghostty, WezTerm, foot, iTerm2, Alacritty and others) report
 * every press and release when asked; there the game walks while an arrow
 * is held and stops when it is let go. Elsewhere upstream's taps are kept.
 *
 * This file is in the public domain.
 */
#include <curses.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "av_input.h"

static int kitty;            /* the terminal reports presses and releases */
static int presses[16], npress;
static int releases[16], nrelease;

static void term_write(const char *s)
{
    size_t n = strlen(s);
    while (n > 0) {
        ssize_t w = write(STDOUT_FILENO, s, n);
        if (w <= 0)
            return;
        s += w;
        n -= (size_t) w;
    }
}

static int readbyte(int ms)
{
    int c;
    timeout(ms);
    c = getch();
    timeout(0);
    return c;
}

static void push(int *q, int *n, int key)
{
    if (*n < 16)
        q[(*n)++] = key;
}

static int pop(int *q, int *n)
{
    int key;
    if (*n == 0)
        return ERR;
    key = q[0];
    memmove(q, q + 1, (size_t) (--*n) * sizeof *q);
    return key;
}

static void restore(void)
{
    if (kitty) {
        term_write("\033[<u");
        kitty = 0;
    }
}

static void on_signal(int sig)
{
    restore();
    endwin();
    signal(sig, SIG_DFL);
    raise(sig);
}

/* a kitty key report as a curses key */
static int kitty_key(int code, int mods, char final)
{
    int shift = mods & 1, ctrl = mods & 4;

    if (final == 'u') {
        if (ctrl && (code == 'c' || code == 'C'))
            on_signal(SIGINT); /* Ctrl+C still ends the game */
        switch (code) {
        case 13: case 57414: return '\n';
        case 27: return 27;
        case 9: return '\t';
        case 127: return KEY_BACKSPACE;
        case 57417: return KEY_LEFT;
        case 57418: return KEY_RIGHT;
        case 57419: return KEY_UP;
        case 57420: return KEY_DOWN;
        }
        if (code >= 57344)
            return ERR; /* modifiers and other functional keys */
        if (shift && code >= 'a' && code <= 'z')
            return code - 'a' + 'A';
        return code;
    }
    if (final == '~') {
        switch (code) {
        case 19: return KEY_F(8);
        }
        return ERR;
    }
    switch (final) {
    case 'A': return KEY_UP;
    case 'B': return KEY_DOWN;
    case 'C': return KEY_RIGHT;
    case 'D': return KEY_LEFT;
    }
    return ERR;
}

/* one key report, after its ESC */
static void kitty_event(void)
{
    /* code ; modifiers:event ; text, each defaulting when left out */
    int f[3][2] = { { 1, 0 }, { 1, 1 }, { 0, 0 } };
    int seen[3][2] = { { 0 } };
    int field = 0, sub = 0, c, key, event, reply = 0;
    char final = 0;

    c = readbyte(50);
    if (c == ERR) {
        push(presses, &npress, 27);
        return;
    }
    if (c == 'O') { /* SS3: arrows and F1 to F4 the old way */
        c = readbyte(50);
        key = c == ERR ? ERR : kitty_key(1, 0, (char) c);
        if (key != ERR)
            push(presses, &npress, key);
        return;
    }
    if (c != '[')
        return;
    for (;;) {
        c = readbyte(50);
        if (c == ERR)
            return;
        if (c >= 0x40 && c <= 0x7e) {
            final = (char) c;
            break;
        }
        if (c == '?' || c == '>') {
            reply = 1; /* an answer to a query, not a key */
        } else if (c == ';') {
            field++;
            sub = 0;
        } else if (c == ':') {
            sub++;
        } else if (c >= '0' && c <= '9' && field < 3 && sub < 2) {
            if (!seen[field][sub]) {
                f[field][sub] = 0;
                seen[field][sub] = 1;
            }
            f[field][sub] = f[field][sub] * 10 + (c - '0');
        }
    }
    if (reply)
        return;
    key = kitty_key(f[0][0], (f[1][0] - 1) & ~(64 | 128), final);
    event = f[1][1]; /* 1 press, 2 repeat, 3 release */
    if (key == ERR)
        return;
    if (event == 3)
        push(releases, &nrelease, key);
    else if (event == 1)
        push(presses, &npress, key);
}

static void drain(void)
{
    int c;

    while ((c = readbyte(0)) != ERR) {
        if (kitty && c == 27)
            kitty_event();
        else if (c != KEY_RESIZE)
            push(presses, &npress, c);
    }
}

void av_input_init(void)
{
    char reply[64];
    int n = 0, supported = 0, c;

    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO))
        return;
    /* ask for the protocol's flags, then for the device attributes, which
       every terminal answers: a flags reply first means kitty support */
    keypad(stdscr, FALSE);
    term_write("\033[?u\033[c");
    while ((c = readbyte(500)) != ERR) {
        if (n < (int) sizeof reply - 1)
            reply[n++] = (char) c;
        reply[n] = 0;
        if (c == 'u' && strstr(reply, "\033[?"))
            supported = 1;
        if (c == 'c' && strstr(reply, "\033[?"))
            break;
    }
    if (supported) {
        /* 1 disambiguate, 2 event types, 8 all keys as codes */
        term_write("\033[>11u");
        kitty = 1;
        atexit(restore);
        signal(SIGINT, on_signal);
        signal(SIGTERM, on_signal);
        signal(SIGHUP, on_signal);
    } else {
        keypad(stdscr, TRUE);
    }
}

void av_input_end(void)
{
    restore();
}

int av_input_releases(void)
{
    return kitty;
}

int av_getkey(void)
{
    drain();
    return pop(presses, &npress);
}

int av_getrelease(void)
{
    return pop(releases, &nrelease);
}
