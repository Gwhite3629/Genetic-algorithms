#ifndef _CREATURE_HPP_
#define _CREATURE_HPP_

#include "trait.hpp"

class Creature
{
public:
    unsigned int tag;
    char *tag_str;
    int n_traits = 0;

    Creature(int,char*,bool);
    ~Creature();

    unsigned int generate_tag();
    int add_trait(Trait);

private:
    int max_energy; // Creatures generational energy
    int energy;     // Creatures current energy
    Trait *trait;   // Array of traits
    char *name;     // Name of creature type
    bool evolve;    // Indicates if the creatures traits matter
};

#endif // _CREATURE_H_
