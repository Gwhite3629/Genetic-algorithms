#include "trait.h"
#include "utils.h"

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
    MEM(trait->name, strlen(name)+1, char);
    strcpy(trait->name, name);

exit:
    return ret;
}

void destroy_trait(Trait *trait)
{
    SFREE(trait->name);
}
