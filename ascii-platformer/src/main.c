#include <stdio.h>
#include <curses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include "level.h"
#include "map.h"
#include "entity.h"
#include "enemy.h"
#include "helper.h"

#define PROJECT_NAME "linux-game-jam-2019-game"
#define KEY_SPACEBAR ' '

struct Entity player;


WINDOW *infowin;
WINDOW *mainwin;

void process(double delta, chtype input, SDL_Event ev);

SDL_GameController* gamepad = NULL;

int debug = 0;
int quit = 0;

Mix_Music *mus = NULL;

int main(int argc, char **argv) {
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

    SDL_Event ev;

    if(SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL audio could not initialize! SDL Error: %s\n", SDL_GetError());
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    mus = Mix_LoadMUS("data/midnight-tale-by-kevin-macleod.ogg" );
    if( mus == NULL ) {
        SDL_Log( "Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
    }

    initHelp(mainwin);

    if (argc > 1)
        setCurrentLevel(loadLevel(argv[1], &player));
    else
        setCurrentLevel(loadLevel("menu", &player));
    
    //setCurrentLevel(level->name);

    initscr();
    resize_term(25, 80);
    nodelay(stdscr, TRUE);
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    infowin = newwin(1, 80, 0, 0);
    werase(infowin);
    mainwin = newwin(24, 80, 1, 0);
    werase(mainwin);

    chtype input;

    clock_t now = 0;
    clock_t last = clock();
    time_t sec;
    time(&sec);
    time_t lastsec = sec;
    int fps = 0;
    int frames = 0;

    double delta = 0;
    double slack = 0;

    char* debugstr = (char *)malloc(44);

    Mix_PlayMusic(mus, -1);

    while (!quit) {
        input = getch();
        
        struct Level* level = getCurrentLevel();

        struct Map* map = level->map;

        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                mvwaddch(mainwin, y, x, map->data[y][x]);
            }
        }

        for (int i = 0; i < MAX_ENTITIES; i++) {
            if (level->enemies[i] != NULL) {
                if (!level->enemies[i]->disabled && !level->enemies[i]->hidden) {
                    for (int j = 0; j < level->enemies[i]->width; j++) {
                        mvwaddch(mainwin, level->enemies[i]->y, level->enemies[i]->x + j, level->enemies[i]->sprite);
                    }
                }
            }
        }

        if (player.state != DEAD)
            mvwaddch(mainwin, player.y, player.x, '@');
        else
            mvwaddch(mainwin, player.y, player.x, 'X');

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
            SDL_Delay((long) (slack * 1000));
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

        for (int i = 0; i < MAX_ENTITIES; i++) {
            if (level->enemies[i] != NULL) {
                if (!level->enemies[i]->disabled) {
                    level->enemies[i]->processFunc(level->enemies[i], delta);
                }
            }
        }

        process(delta, input, ev);
        wrefresh(mainwin);
    }

    free(debugstr);
    endwin();

    Mix_FreeMusic(mus);
    mus = NULL;
    Mix_Quit();

    if (gamepad != NULL)
        SDL_GameControllerClose(gamepad);

    SDL_Quit();

    return 0;
}

void process(double delta, chtype input, SDL_Event ev) {
    struct Level* level = getCurrentLevel();

    switch(input) {
        case ERR:
            break;
        case 'r':
            switchLevel(level->name, &player);
            werase(mainwin);
            player.state = WALKING;
            break;
        case KEY_F(8):
            debug = !debug;
            werase(infowin);
            break;
        case 'q':
            if(Mix_PlayingMusic() == 0) {
                Mix_PlayMusic(mus, -1);
            } else {
                Mix_HaltMusic();
            }
    }

    Uint8 btn;

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            quit = 1;
            return;
        } else if (ev.type == SDL_CONTROLLERBUTTONDOWN) {
            btn = ev.cbutton.button;
            if (btn == SDL_CONTROLLER_BUTTON_Y) {
                switchLevel(level->name, &player);
                werase(mainwin);
                player.state = WALKING;
                break;
            } else if (player.state != DEAD) {
                if (btn == SDL_CONTROLLER_BUTTON_A) {
                    if (player.state == WALKING) {
                        player.y_vel = -8;
                        player.state = AIRBORNE;
                    }
                }
                if (btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
                    player.x_vel = -8;
                } else if (btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                    player.x_vel = 8;
                }
            }
        } else if (ev.type == SDL_CONTROLLERBUTTONUP && player.state != DEAD) {
            btn = ev.cbutton.button;
            if ((btn == SDL_CONTROLLER_BUTTON_DPAD_LEFT && player.x_vel < 0) ||
                (btn == SDL_CONTROLLER_BUTTON_DPAD_RIGHT && player.x_vel > 0)) {
                player.x_vel = 0;
            }
        }
    }

    if (player.state == DEAD)
        return;

    switch(input) {
        case ERR:
            break;
        case KEY_SPACEBAR:
        case KEY_UP:
            if (player.state == WALKING) {
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
