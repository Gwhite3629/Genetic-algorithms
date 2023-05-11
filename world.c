#include "world.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int new_world(World *world, unsigned int xsize, unsigned int ysize)
{
    int ret = SUCCESS;

    world->x_size = xsize;
    world->y_size = ysize;
    world->world = new(world->world, xsize*ysize, unsigned int);
    world->creatures = new(world->creatures, 1, hashmap_t);
    world->n_creatures = 0;

exit:
    return ret;
}

void destroy_world(World *world)
{
    int ret = SUCCESS;
    for (int i = 0; i < world->n_creatures; i++) {
        destroy_creature((Creature *)world->creatures[i].data);
        del(world->creatures[i].string);
    }
    del(world->world);
    del(world->creatures);
exit:
    return;
}

int run()
{
    return 0;
}

unsigned int check_world(World *world, int x, int y)
{
    return world->world[y*world->x_size+x];
}
