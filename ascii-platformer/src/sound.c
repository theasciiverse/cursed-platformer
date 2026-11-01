#include <stdbool.h>
#include <stdint.h>

#ifndef NOSDL
#include <SDL.h>
#include <SDL_mixer.h>

Mix_Music *mus = NULL;
Mix_Chunk *sounds[6];
#endif

bool sound_enabled = true;

bool initAudio(void) {
#ifndef NOSDL
    static bool opened = false;
    if (opened)
        return sound_enabled; /* main() asks twice */
    opened = true;
    if(SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL audio could not initialize! SDL Error: %s\n", SDL_GetError());
        sound_enabled = false;
        return false;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        sound_enabled = false;
        return false;
    }

    mus = Mix_LoadMUS("data/audio/midnight-tale-by-kevin-macleod.ogg" );

    sounds[0] = Mix_LoadWAV("data/audio/jump.ogg");
    sounds[1] = Mix_LoadWAV("data/audio/death.ogg");
    sounds[2] = Mix_LoadWAV("data/audio/goal.ogg");
    sounds[3] = Mix_LoadWAV("data/audio/rubble.ogg");
    sounds[4] = Mix_LoadWAV("data/audio/pressureplate.ogg");
    sounds[5] = Mix_LoadWAV("data/audio/sproing.ogg");

    if (mus == NULL || sounds[0] == NULL || sounds[1] == NULL || sounds[2] == NULL) {
        SDL_Log( "Failed to load music or sounds effects!\nSDL_mixer Error: %s\n", Mix_GetError());
        sound_enabled = false;
        return false;
    } else {
        Mix_PlayMusic(mus, -1);
    }
#endif
    return true;
}

void toggleMusic(void) {
#ifndef NOSDL
    if (!sound_enabled)
        return;
    if(Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(mus, -1);
    } else {
        Mix_HaltMusic();
    }
#endif
}

void playSound(int snd) {
#ifndef NOSDL
    if (!sound_enabled)
        return;
    Mix_PlayChannel(-1, sounds[snd], 0);
#endif
}

void setSoundVolume(int snd, int volume) {
#ifndef NOSDL
    if (!sound_enabled)
        return;
    if (volume > MIX_MAX_VOLUME)
        volume = MIX_MAX_VOLUME;
    Mix_VolumeChunk(sounds[snd], volume);
#endif
}

void closeAudio(void) {
#ifndef NOSDL
    if (mus != NULL) {
        Mix_FreeMusic(mus);
        mus = NULL;
    }
    for (unsigned int i = 0; i < sizeof(sounds) / sizeof(sounds[0]); i++) {
        if (sounds[i] != NULL) {
            Mix_FreeChunk(sounds[i]);
            sounds[i] = NULL;
        }
    }

    Mix_Quit();
#endif
}
