#include "hashmap.h"
#include "creature.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <assert.h>

Creature::Creature(int max_energy, char *name, bool evolve)
{
    int ret = SUCCESS;

    this->max_energy = max_energy;
    this->energy = max_energy;
    MEM(this->name, strlen(name), char);
    strcpy(this->name, name);
    this->evolve = evolve;

    MEM(this->trait, 1, Trait);
exit:
    assert(ret == 0);
}

Creature::~Creature()
{
    SFREE(trait);
    SFREE(name);
    SFREE(tag_str);
}

unsigned int Creature::generate_tag()
{
    int ret = SUCCESS;
    int i = 0;

    MEM(this->tag_str, 1, char);

    while ((this->trait + i) != NULL) {
        MEM_(this->tag_str, strlen(this->trait[i].name), char);
        strcat(this->tag_str, this->trait[i].name);
        i++;
    }

    ret = hash(this->tag_str);

exit:
    return (unsigned int) ret;
}

int Creature::add_trait(Trait new_trait)
{
    int ret = SUCCESS;

    MEM_(this->trait, this->n_traits+1, Trait);
    this->trait[this->n_traits] = new_trait;
    this->n_traits++;

exit:
    return ret;
}