#include "trait.h"
#include "utils.h"
#include "memory.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int new_trait(Trait *trait, char *name)
{
    int ret = SUCCESS;

    if (strlen(name) == 0) {
        return ret;
    }
    trait->name = new(trait->name, strlen(name)+1, char);
    strcpy(trait->name, name);
    trait->name[strlen(name)] = '\0';
    trait->n_params = 0;
    trait->params = new(trait->params, 1, int);

exit:
    return ret;
}

int add_param(Trait *trait, int param)
{
    int ret = SUCCESS;

    trait->n_params++;
    trait->params = widen(trait->params, trait->n_params, int);
    trait->params[trait->n_params-1] = param;

exit:
    return ret;
}

void destroy_trait(Trait *trait)
{
    int ret = SUCCESS;
    del(trait->params)
    del(trait->name);
exit:
    return;
}
