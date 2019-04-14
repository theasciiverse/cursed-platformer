#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

struct Map {
    int width, height;
    int start_x, start_y;
    char** data;
};

struct Map* loadMap(char* name);

void destroyMap(struct Map* map);

#endif
