#ifndef CREATURE_H
#define CREATURE_H

#include "trait.h"

class Creature
{
public:
    unsigned int tag;

    Creature();
    unsigned int generate_tag();

private:
    int max_energy; // Creatures generational energy
    int energy;     // Creatures current energy
    Trait *trait;   // Array of traits
    char *name;     // Name of creature type
    bool evolve;    // Indicates if the creatures traits matter
}

#endif // CREATURE_H
