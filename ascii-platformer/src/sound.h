#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include <stdbool.h>

#include <SDL.h>
#include <SDL_mixer.h>

#define SND_JUMP 0
#define SND_DEATH 1
#define SND_GOAL 2
#define SND_RUBBLE 3
#define SND_PRESSUREPLATE 4
#define SND_TRAMPOLINE 5

bool initAudio(void);
void playSound(int snd);
void toggleMusic(void);
void closeAudio(void);

#endif
