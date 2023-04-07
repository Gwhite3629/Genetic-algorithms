#ifndef _CREATURE_H_
#define _CREATURE_H_

#include "trait.h"

typedef struct Creature {
    unsigned int tag;
    char *tag_str;
    int n_traits;
    int max_energy;
    int energy;
    Trait **traits;
    char *name;
    int evolve;
} Creature;

int new_creature(Creature *creature, int max_energy, char *name, int evolve);
void destroy_creature(Creature *creature);
unsigned int generate_tag(Creature *creature);
int add_trait(Creature *creature, char *name);

#endif // _CREATURE_H_