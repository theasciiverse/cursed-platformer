#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

struct Level {
    int width, height;
    char** data;
};

struct Level* load_level(char* name);

#endif
