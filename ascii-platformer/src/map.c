#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Map {
    int width, height;
    int start_x, start_y;
    char** data;
};

struct Map* loadMap(char* name) {
    char metadata_path[80];
    strncpy(metadata_path, name, 80 - strlen("/metadata.txt") - strlen(name));
    strcat(metadata_path, "/metadata.txt");
    
    char geometry_path[80];
    strncpy(geometry_path, name, 80 - strlen("/map.txt") - strlen(name));
    strcat(geometry_path, "/map.txt");
    
    printf("Loading level data from: %s\n", &geometry_path);

    FILE* metadata = fopen(metadata_path, "r");
    FILE* fp = fopen(geometry_path, "r");
    
    int width, height;
    int start_x, start_y;

    char buf[80];

    fscanf(metadata, "%s %d %s %d", &buf, &width, &buf, &height);
    fscanf(metadata, "%s %d %s %d", &buf, &start_x, &buf, &start_y);
    
    printf("Height: %d, Width: %d\n", width, height);
    printf("Start X: %d, Start Y: %d\n", start_x, start_y);

    fclose(metadata);

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
    map->start_x = start_x;
    map->start_y = start_y;
    map->data = data;

    return map;
}

void destroyMap(struct Map* map) {
    free(map->data);
    free(map);
}
