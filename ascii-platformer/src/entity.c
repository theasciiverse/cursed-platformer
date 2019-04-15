#include "level.h"
#include "map.h"
#include "helper.h"

void processPhysics(struct Entity* entity, double delta) {
    int old_x = (int) entity->x;
    int old_y = (int) entity->y;
    
    struct Map* map = entity->level->map;

    if (entity->x_vel < 0 && (map->data[old_y][old_x - 1] == '#' || map->data[old_y][old_x - 1] == '^')) {
        entity->x = (float) old_x;
        entity->x_vel = 0;
    }
    
    if (entity->x_vel > 0 && (map->data[old_y][old_x + 1] == '#' || map->data[old_y][old_x + 1] == '^')) {
        entity->x = (float) old_x;
        entity->x_vel = 0;
    }

    entity->y += entity->y_vel * delta;
    entity->x += entity->x_vel * delta;

    int new_x = (int) entity->x;
    
    if (map->data[old_y][new_x] == '#' || map->data[old_y][new_x] == '^') {
        entity->x = (float) old_x;
        new_x = old_x;
    }

    int new_y = (int) entity->y;

    if (map->data[new_y][new_x] == '#' || map->data[new_y][new_x] == '^') {
        entity->y = (float) old_y;
        new_y = old_y;
    }

    if (map->data[new_y + 1][new_x] != '#' || map->data[new_y + 1][new_x] != '"') {
        entity->y_vel += 4 * delta;
    }
 
    char below = map->data[new_y + 1][new_x];
    if ((below == '#' || below == '"') && entity->y_vel >= 0) {
        entity->state = WALKING;
        entity->y_vel = 0;
        entity->y = (float) new_y;
    } else if (below == '^') {
        entity->state = DEAD;
        entity->x = (float) new_x;
        entity->y = (float) new_y;
    } else {
        entity->y_vel += 12 * delta;
        entity->state = AIRBORNE;
    }
}

void destroyEntity(struct Entity* entity) {
    free(entity->internal_data);
    free(entity);
}

void processMessage(struct Entity* message, double delta);

struct Entity* createMessage(struct Level* level, double x, double y, char* who, char* msg, int read) {
    struct Entity* message = malloc(sizeof(struct Entity));

    message->x = x;
    message->y = y;
    message->x_vel = 0;
    message->sprite = '?';
    message->state = IDLE; 
    message->disabled = false;
    message->hidden = false;
    message->internal_data = malloc(sizeof(struct Message));
    
    struct Message* message_data = (struct Message*) message->internal_data;

    message->level = level;
    message_data->who = who;
    message_data->msg = msg;
    message_data->read = read; 

    message->processFunc = &processMessage;
    return message;
}

void processMessage(struct Entity* message, double delta) {
    int x = (int) message->x;
    int y = (int) message->y;

    int px = (int) message->level->player->x;
    int py = (int) message->level->player->y;

    struct Message* message_data = (struct Message*) message->internal_data;
    
    if (!message_data->read && x == px && y == py) {
        showMessage(getMainWin(), message_data->who, message_data->msg);       
        message_data->read = true;
        message->disabled = true;
        message->sprite = '!';
    }
}

void processButton(struct Entity* message, double delta);

struct Entity* createButton(struct Level* level, double x, double y, double tx, double ty) {
    struct Entity* button = malloc(sizeof(struct Entity));

    button->x = x;
    button->y = y;
    button->x_vel = 0;
    button->sprite = '_';
    button->state = IDLE; 
    button->disabled = false;
    button->hidden = false;
    button->internal_data = malloc(sizeof(struct Button));
    
    struct Button* button_data = (struct Button*) button->internal_data;

    button->level = level;
    button_data->tx = tx;
    button_data->ty = ty; 

    button->processFunc = &processButton;
    return button;
}

void processButton(struct Entity* button, double delta) {
    int x = (int) button->x;
    int y = (int) button->y;

    int px = (int) button->level->player->x;
    int py = (int) button->level->player->y;

    struct Button* message_data = (struct Button*) button->internal_data;
    
    if (x == px && y == py) {
        int tx = message_data->tx;
        int ty = message_data->ty;

        button->level->map->data[ty][tx] = ' '; 
        button->disabled = true;
    }
}
