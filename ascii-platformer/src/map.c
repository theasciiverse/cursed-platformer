#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "map.h"
#include "level.h"

struct Map* loadMap(struct Metadata* metadata, char* path) {
    char map_path[80];
    strcpy(map_path, path);
    strcat(map_path, "/map.txt");

    FILE* fp = fopen(map_path, "rb");
    
    struct Map* map = malloc(sizeof(struct Map));

    map->width = metadata->width;
    map->height = metadata->height;



    unsigned map_row_ptr_count = map->height * sizeof(char*);
    char** data = malloc(map_row_ptr_count);
    memset(data, 0x0, map_row_ptr_count);

    for (int i = 0; i < map->height; i++) {
        char* buf = malloc(map->width);
        fread(buf, map->width, 1, fp);
        fseek(fp, 1, SEEK_CUR);
        data[i] = buf;
    }

    fclose(fp);

    map->data = data;

    strcpy(map_path, path);
    strcat(map_path, "/colormap.txt");

    char** cdata = malloc(map_row_ptr_count);
    memset(cdata, 0x0, map_row_ptr_count);
    int use_colors = 0;
    if (access(map_path, F_OK) != -1) {
        fp = fopen(map_path, "rb");
        use_colors = 1;
    }

    for (int i = 0; i < map->height; i++) {
        char* buf = malloc(map->width);
        if (use_colors) {
            fread(buf, map->width, 1, fp);
            fseek(fp, 1, SEEK_CUR);
        } else {
            memset(buf, 0x0, map->width);
        }
        cdata[i] = buf;
    }

    if (use_colors) {
        fclose(fp);
    }
    map->colors = cdata;

    return map;
}

void destroyMap(struct Map* map) {
    for(int i = 0; i < map->height; i++) {
        free(map->data[i]);
        free(map->colors[i]);
    }
    free(map->data);
    free(map->colors);
    free(map);
}
