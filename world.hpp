#ifndef _WORLD_HPP_
#define _WORLD_HPP_

#include "creature.hpp"
#include "hashmap.h"

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
    unsigned int x_size;
    unsigned int y_size;
    unsigned int age;

    World(unsigned int,unsigned int);
    ~World();

    int populate(Creature);

    int run();

    unsigned int check_world(int,int);

    int get_stat();

private:
    unsigned int *world;
    hashmap_t *creatures;

};

#endif // _WORLD_H_
