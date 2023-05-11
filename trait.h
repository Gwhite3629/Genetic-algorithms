#ifndef _TRAIT_H_
#define _TRAIT_H_

typedef struct Trait {
    char *name;     // Name of trait
    int n_params;
    int *params;
} Trait;

int new_trait(Trait *trait, char *name);
int add_param(Trait *trait, int param);
void destroy_trait(Trait *trait);

#endif // _TRAIT_H_
