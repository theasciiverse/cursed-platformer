#include <curses.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef NOSDL
#include <SDL.h>
#endif

#include "level.h"
#include "map.h"
#include "entity.h"
#include "enemy.h"
#include "helper.h"
#include "sound.h"

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

struct Entity player;

WINDOW *infowin;
WINDOW *mainwin;

#ifndef NOSDL
void process(double delta, chtype input, SDL_Event ev);
int initSDL();
void deinitSDL();
SDL_GameController* gamepad = NULL;
#else 
void process(double delta, chtype input);
#endif 

int debug = 0;
int quit = 0;

int use_color = 1;

int main(int argc, char **argv) {
#ifdef USE_PDCURSES
    SDL_setenv("PDC_FONT_SIZE", "22", 1);
    SDL_setenv("PDC_FONT", "data/Px437_AmstradPC1512-2y.ttf", 1);
#endif
#ifndef NOSDL
    initSDL();
    SDL_Event ev;
#endif

    if (argc > 1)
        setCurrentLevel(loadLevel(argv[1], &player));
    else
        setCurrentLevel(loadLevel("menu", &player));

    initAudio();
    
    int width = 80;
    int height = 25;

    initscr();
    start_color();
    nodelay(stdscr, TRUE);
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_YELLOW, COLOR_BLACK);
    
    infowin = newwin(1, width, 0, 0);
    werase(infowin);
    mainwin = newwin(height - 1, width, 1, 0);
    initHelp(mainwin);
    
    werase(mainwin);
    
    touchwin(mainwin);

    if (argc > 1) {
        setCurrentLevel(loadLevel(argv[1], &player));
    } else {
        setCurrentLevel(loadLevel("menu", &player));
    }
    initAudio();
    chtype input;

    clock_t now = 0;
    clock_t last = clock();
    time_t sec;
    time(&sec);
    time_t lastsec = sec;
    int fps = 0;
    int frames = 0;
    double slack = 0;
    double delta = 0;

    int x_offset = 0;
    int y_offset = 0;

    char* debugstr = (char *)malloc(44);

    while (!quit) {
        input = getch();
        
        struct Level* level = getCurrentLevel();

        struct Map* map = level->map;

        int color = 0;
        
        x_offset = MIN(player.x - (width / 2), map->width - width);
        x_offset = MAX(x_offset, 0);

        y_offset = MIN(player.y - (height / 2), map->height - height);
        y_offset = MAX(y_offset, 0);
        
        int x_max = MIN(width, map->width);
        int y_max = MIN(height, map->height);

        for (int y = 0; y < y_max; y++) {
            for (int x = 0; x < x_max; x++) {
                if (use_color) {
                    if (map->colors[y + y_offset][x + x_offset] != color) {
                        if (color != 0) {
                            wattroff(mainwin, COLOR_PAIR(color - '0'));
                        }
                        color = map->colors[y + y_offset][x + x_offset];
                        if (color != 0) {
                            wattron(mainwin, COLOR_PAIR(color - '0'));
                        }
                    }
                }
                mvwaddch(mainwin, y, x, map->data[y + y_offset][x + x_offset]);
            }
        }
        if (color != 0 && use_color) {
            wattroff(mainwin, COLOR_PAIR(color - '0'));
            color = 0;
        }
        
        for (int i = 0; i < MAX_ENTITIES; i++) {
            if (level->enemies[i] != NULL) {
                if (!level->enemies[i]->disabled && !level->enemies[i]->hidden) {
                    for (int j = 0; j < level->enemies[i]->width; j++) {
                        if (level->enemies[i]->color != 0)
                            wattron(mainwin, COLOR_PAIR(level->enemies[i]->color));
                        mvwaddch(mainwin, level->enemies[i]->y - y_offset, level->enemies[i]->x + j - x_offset, level->enemies[i]->sprite);
                        if (level->enemies[i]->color != 0)
                            wattroff(mainwin, COLOR_PAIR(level->enemies[i]->color));
                    }
                }
            }
        }

        if (player.state != DEAD)
            mvwaddch(mainwin, player.y - y_offset, player.x - x_offset, '@');
        else {
            wattron(mainwin, COLOR_PAIR(2));
            mvwaddch(mainwin, player.y - y_offset, player.x - x_offset, 'X');
            wattroff(mainwin, COLOR_PAIR(2));
        }

        time(&sec);

        if (sec - lastsec > 0) {
            fps = frames;
            frames = 0;
            lastsec = sec;
        }
        frames++;

        now = clock();

        delta = ((double) (now - last) / CLOCKS_PER_SEC);
        slack = (1.0 / 60) - delta;
        if (slack > 0) {
#ifndef NOSDL
            SDL_Delay((long) (slack * 1000));
#else
            usleep(slack * 1000000);
#endif
            delta += slack;
        }
        last = now;

        mvwaddstr(infowin, 0, 0, "LEVEL ");
        mvwaddnstr(infowin, 0, 6, level->metadata->title, 70);
        if (debug) {
            char* statestr = (player.state == WALKING) ? "WALKING" : (player.state == AIRBORNE) ? "AIRBORNE" : "DEAD";
            snprintf(debugstr, 44, "FPS:%-3d X:%-9f Y:%-9f ST:%-8s", fps, player.x, player.y, statestr);
            mvwaddnstr(infowin, 0, 36, debugstr, 44);
        }
        wrefresh(infowin);

        int stop = 0;
        for (int i = 0; i < MAX_ENTITIES && !stop; i++) {
            if (level->enemies[i] != NULL) {
                if (!level->enemies[i]->disabled) {
                    switch(level->enemies[i]->processFunc(level->enemies[i], delta)) {
                        case the_level_changed : 
                            stop = 1;
                            break;
                        case pauze_timing:
                            last = clock();
                            delta = 0;
                            break;
                        case nothing_to_report :
                        default :
                            break;
                    }
                }
            } else {
                break;
            }
        }
#ifndef NOSDL
        process(delta, input, ev);
#else 
        process(delta, input);
#endif
        wrefresh(mainwin);
    }

    delwin(mainwin);
    delwin(infowin);
    free(debugstr);
    endwin();

#ifndef NOSDL
    deinitSDL();
#endif

    return 0;
}
#ifndef NOSDL
void process(double delta, chtype input, SDL_Event ev) 
#else
void process(double delta, chtype input) 
#endif
{
    struct Level* level = getCurrentLevel();

    switch(input) {
        case ERR:
            break;
        case 'r':
            switchLevel(level->name, &player);
            werase(mainwin);
            player.state = WALKING;
            break;
        case 'q':
            quit = 1;
            break;
        case KEY_F(8):
            debug = !debug;
            werase(infowin);
            break;
        case 'm':
            toggleMusic();
            break;
        case 'c':
            use_color = (use_color == 1) ? 0 : 1;
    }

#ifndef NOSDL
    Uint8 btn;
    SDL_Keycode key;

    SDL_PumpEvents();
    while (SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_CONTROLLERBUTTONUP)) {
        if (ev.type == SDL_CONTROLLERBUTTONDOWN || ev.type == SDL_CONTROLLERBUTTONUP) {
            btn = ev.cbutton.button;
        } else {
            btn = -1;
        }
        if (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) {
            key = ev.key.keysym.sym;
        } else {
            key = -1;
        }
        if (ev.type == SDL_CONTROLLERBUTTONDOWN || ev.type == SDL_KEYDOWN) {
            if (key == SDLK_q) {
                quit = 1;
                return;
            } else if (btn == SDL_CONTROLLER_BUTTON_Y || key == SDLK_r) {
                switchLevel(level->name, &player);
                werase(mainwin);
                player.state = WALKING;
                return;
            } else if (key == SDLK_c) {
                use_color = (use_color == 1) ? 0 : 1;
            } else if (key == SDLK_m) {
                toggleMusic();
            } else if (key == SDLK_F8) {
                debug = !debug;
                werase(infowin);
            } else if (player.state != DEAD) {
                if (btn == SDL_CONTROLLER_BUTTON_A || key == SDLK_SPACE || key == SDLK_UP) {
                    if (player.state == WALKING) {
                        playSound(SND_JUMP);
                        player.y_vel = -8;
                        player.state = AIRBORNE;
                    }
                }
                if (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT || key == SDLK_LEFT) {
                    player.x_vel = -8;
                } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT || key == SDLK_RIGHT) {
                    player.x_vel = 8;
                }
            }
        } else if ((ev.type == SDL_CONTROLLERBUTTONUP || ev.type == SDL_KEYUP) && player.state != DEAD) {
            if (((btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT || key == SDLK_LEFT) && player.x_vel < 0) ||
                ((btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT || key == SDLK_RIGHT) && player.x_vel > 0)) {
                player.x_vel = 0;
            }
        }
    }
#endif

    if (player.state == DEAD)
        return;

    switch(input) {
        case ERR:
            break;
        case KEY_SPACEBAR:
        case KEY_UP:
            if (player.state == WALKING) {
                playSound(SND_JUMP);
                player.y_vel = -8;
                player.state = AIRBORNE;
            }
            break;
        case KEY_LEFT:
            if (player.x_vel < 0)
                player.x_vel = 0;
            else
                player.x_vel = -8;
            break;
        case KEY_RIGHT:
            if (player.x_vel > 0)
                player.x_vel = 0;
            else
                player.x_vel = 8;
    }

    processPhysics(&player, delta);
}
#ifndef NOSDL
int initSDL() {
    if (SDL_WasInit(SDL_INIT_EVENTS) == 0) {
        if (SDL_Init(SDL_INIT_EVENTS) != 0) {
            SDL_Log("Unable to initialize SDL events: %s", SDL_GetError());
            return 1;
        }
    }

    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
            SDL_Log("Unable to initialize SDL game controller: %s", SDL_GetError());
            return 1;
        }
    }

    SDL_GameController *gamepad = NULL;

    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            gamepad = SDL_GameControllerOpen(i);
            if (gamepad != NULL) {
                SDL_Log("Gamepad Found: %s\n", SDL_GameControllerNameForIndex(i));
                break;
            } else {
                SDL_Log("Could not open gamepad %i: %s\n", i, SDL_GetError());
            }
        }
    }
    return 0;
}

void deinitSDL() {
    if (gamepad != NULL)
        SDL_GameControllerClose(gamepad);
    SDL_Quit();
}
#endif
