#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Map {
    int width, height;
    char** data;
};

struct Map* loadMap(char* name) {
    FILE* fp = fopen(name, "r");

    int width, height;

    fscanf(fp, "%d %d", &width, &height);
    
    width += 1;

    char** data = malloc(sizeof(char*) * (width * height));
    
    for (int i = 0; i < height; i++) {
        char* buf = malloc(width);

        fgets(buf, width, fp);
        data[i] = buf;
    }

    fclose(fp);

    struct Map* map = malloc(sizeof(struct Map));
    map->width = width;
    map->height = height;
    map->data = data;

    return map;
}

void destroyMap(struct Map* map) {
    free(map->data);
    free(map);
}
