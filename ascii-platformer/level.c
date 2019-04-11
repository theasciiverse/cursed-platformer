#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Level {
    int width, height;
    char** data;
};

struct Level* load(char* name) {
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

    struct Level* level = malloc(sizeof(struct Level));
    level->width = width;
    level->height = height;
    level->data = data;

    return level;
}
