#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "map.h"
#include "level.h"

struct Map* createMap(struct Metadata* metadata, int colors);

struct Map* loadMap(struct Metadata* metadata, char* path) {
    int use_colors = 0;
    char map_path[80];
    char color_path[80];

    strcpy(color_path, path);
    strcat(color_path, "/colormap.txt");

    if (access(color_path, F_OK) != -1) {
        use_colors = 1;
    }
    struct Map* map = createMap(metadata, use_colors);
    strcpy(map_path, path);
    strcat(map_path, "/map.txt");

    FILE* fp = fopen(map_path, "rb");


    for (int i = 0; i < map->height; i++) {
        fread(map->data[i], map->width, 1, fp);
        fseek(fp, 1, SEEK_CUR);
    }

    fclose(fp);

    if(use_colors) {
        fp = fopen(color_path, "rb");
    }

    for (int i = 0; i < map->height; i++) {
        if (use_colors) {
            fread(map->colors[i], map->width, 1, fp);
            fseek(fp, 1, SEEK_CUR);
        } else {
            memset(map->colors[i], 0x0, map->width);
        }
    }

    if (use_colors) {
        fclose(fp);
    }

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

struct Map* createMap(struct Metadata* metadata, int colors) {
    struct Map* map = malloc(sizeof(struct Map));
    map->width = metadata->width;
    map->height = metadata->height;

    unsigned map_row_ptr_count = map->height * sizeof(char*);
    map->data = malloc(map_row_ptr_count);
    memset(map->data, 0x0, map_row_ptr_count);
    if(colors) {
        map->colors = malloc(map_row_ptr_count);
        memset(map->colors, 0x0, map_row_ptr_count);
    } else {
        map->colors = NULL;
    }

    for (int i = 0; i < map->height; i++) {
        map->data[i] = malloc(map->width+1);
        memset(map->data[i], 0x0, map->width+1);
        if(colors) {
            map->colors[i] = malloc(map->width+1);
            memset(map->colors[i], 0x0, map->width+1);
        }
    }

    return map;
}

