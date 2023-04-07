#include "trait.hpp"
#include "utils.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <assert.h>

Trait::Trait(char *name)
{
    int ret = SUCCESS;

    MEM(this->name, strlen(name), char);
    strcpy(this->name, name);

exit:
    assert(ret != 0);
}

Trait::~Trait()
{
    SFREE(this->name);
}