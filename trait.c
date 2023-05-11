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

exit:
    return ret;
}

void destroy_trait(Trait *trait)
{
    int ret = SUCCESS;
    del(trait->name);
exit:
    return;
}
