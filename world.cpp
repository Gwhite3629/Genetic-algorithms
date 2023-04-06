#include "world.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <assert.h>

World::World(unsigned int x_size, unsigned int y_size)
{
    int ret = SUCCESS;

    this->x_size = x_size;
    this->y_size = y_size;
    MEM(this->world, x_size*y_size, unsigned int);
    MEM(this->creatures, 1, hashmap_t);
    this->n_creatures = 0;

exit:
    assert(ret == 0);
}

int World::populate(Creature *new_creature)
{
    int ret = SUCCESS;

    map_insert(&this->creatures, &n_creatures, new_creature->tag_str, new_creature);

exit:
    return ret;
}

int World::run()
{
    int ret = SUCCESS;

exit:
    return ret;
}

unsigned int World::check_world(int x,int y)
{
    return this->world[y*this->x_size+x];
}

int World::get_stat()
{
    int ret = SUCCESS;

exit:
    return ret;
}