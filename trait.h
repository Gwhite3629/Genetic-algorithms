#ifndef _TRAIT_H_
#define _TRAIT_H_

typedef struct Trait {
    char *name;     // Name of trait
} Trait;

int new_trait(Trait *trait, char *name);
void destroy_trait(Trait *trait);

#endif // _TRAIT_H_
