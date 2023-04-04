#ifndef WORLD_H
#define WORLD_H

#include "creature.h"

struct stat
{
    int *n_pop;
    unsigned int *tags;
};

class World
{
public:
    int n_creatures;
    stat creature_stat;

    World();

    int populate(Creature *);

    int run();

    unsigned int check_world(int,int);

    int get_stat();

private:
    unsigned int *world;
    hashmap_t *creatures;

}

#endif // WORLD_H
