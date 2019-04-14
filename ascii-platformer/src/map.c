#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "level.h"

struct Map* loadMap(struct Metadata* metadata, char* name) {
    FILE* fp = fopen(name, "r");
    
    struct Map* map = malloc(sizeof(struct Map));

    map->width = metadata->width;
    map->height = metadata->height;

    unsigned map_row_ptr_count = map->height * sizeof(char*);
    char** data = malloc(map_row_ptr_count);
    memset(data, 0x0, map_row_ptr_count);
    
    for (int i = 0; i < map->height; i++) {
        char* buf = malloc(map->width);
        int eol;
        fread(buf, map->width, 1, fp);
        eol = fgetc(fp); // \n
        data[i] = buf;
    }

    fclose(fp);

    map->data = data;

    return map;
}

void destroyMap(struct Map* map) {
    for(int i = 0; i < map->height; i++) {
        free(map->data[i]);
    }
    free(map->data);
    free(map);
}
