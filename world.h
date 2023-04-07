#ifndef _WORLD_H_
#define _WORLD_H_

#include "creature.h"
#include "hashmap.h"

typedef struct World {
    int n_creatures;
    unsigned int x_size;
    unsigned int y_size;
    unsigned int age;
    unsigned int *world;
    hashmap_t *creatures;
} World;


int new_world(World *world, unsigned int xsize, unsigned int ysize);

void destroy_world(World *world);

int run();

unsigned int check_world(World *world, int x, int y);

#endif // _WORLD_H_
