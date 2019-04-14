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

    map->width += 1;

    char** data = malloc(sizeof(char*) * (map->width * map->height));
    
    for (int i = 0; i < map->height; i++) {
        char* buf = malloc(map->width);

        fgets(buf, map->width, fp);
        data[i] = buf;
    }

    fclose(fp);

    map->data = data;

    return map;
}

void destroyMap(struct Map* map) {
    free(map->data);
    free(map);
}
