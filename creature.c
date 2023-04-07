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
    MEM(creature->name, strlen(name), char);
    strcpy(creature->name, name);
    creature->evolve = evolve;
    creature->n_traits = 0;
    MEM(creature->traits, creature->n_traits+1, Trait *);

exit:
    return ret;
}

void destroy_creature(Creature *creature)
{
    for (int i = 0; i < creature->n_traits; i++) {
        destroy_trait(creature->traits[i]);
    }
    SFREE(creature->traits);
    SFREE(creature->name);
    SFREE(creature->tag_str);
}

unsigned int generate_tag(Creature *creature)
{
    int ret = SUCCESS;
    int len = strlen(creature->name);

    MEM(creature->tag_str, len, char);
    
    strcpy(creature->tag_str, creature->name);

    printf("Ntraits: %d\n", creature->n_traits);

    for (int i = 0; i < creature->n_traits; i++) {
        printf("%d, %s\n", i, creature->traits[i]->name);
        len += strlen(creature->traits[i]->name);
        MEM_(creature->tag_str, len, char);
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

    MEM_(creature->traits, creature->n_traits+1, Trait *);
    MEM(creature->traits[creature->n_traits], 1, Trait);
    new_trait(creature->traits[creature->n_traits], name);
    creature->n_traits++;

exit:
    return ret;
}
