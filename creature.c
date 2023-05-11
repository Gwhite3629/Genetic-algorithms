#include "creature.h"
#include "utils.h"
#include "hashmap.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int new_creature(Creature *creature, int max_energy, char *name, int evolve)
{
    int ret = SUCCESS;

    creature->max_energy = max_energy;
    creature->energy = max_energy;
    creature->name = new(creature->name, strlen(name)+1, char);
    strcpy(creature->name, name);
    creature->name[strlen(name)] = '\0';
    creature->evolve = evolve;
    creature->n_traits = 0;
    creature->traits = new(creature->traits, creature->n_traits+1, Trait *);

exit:
    return ret;
}

void destroy_creature(Creature *creature)
{
    int ret = SUCCESS;
    for (int i = 0; i < creature->n_traits; i++) {
        destroy_trait(creature->traits[i]);
        del(creature->traits[i]);
    }
    del(creature->traits);
    del(creature->name);
    del(creature->tag_str);
    del(creature);
exit:
    return;
}

unsigned int generate_tag(Creature *creature)
{
    int ret = SUCCESS;
    int len = strlen(creature->name)+1;

    creature->tag_str = new(creature->tag_str, len, char);
    
    strcpy(creature->tag_str, creature->name);
    creature->tag_str[len] = '\0';

    for (int i = 0; i < creature->n_traits; i++) {
        len += strlen(creature->traits[i]->name)+1;
        creature->tag_str = widen(creature->tag_str, len, char);
        strcat(creature->tag_str, creature->traits[i]->name);
    }

    ret = hash(creature->tag_str);
    creature->tag = ret;

exit:
    return (unsigned int) ret;
}

int add_trait(Creature *creature, char *name)
{
    int ret = SUCCESS;

    creature->traits = widen(creature->traits, creature->n_traits+1, Trait *);
    creature->traits[creature->n_traits] = new(creature->traits[creature->n_traits], 1, Trait);
    new_trait(creature->traits[creature->n_traits], name);
    creature->n_traits++;

exit:
    return ret;
}
