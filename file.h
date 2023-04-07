#ifndef _FILE_H_
#define _FILE_H_

#include "world.h"

#include <stdio.h>

#define CONFIG_START "################\n\nWORLD_SETTINGS\n"

int read_cfg(World *world);

int find_event(FILE *fd, fpos_t *prof_start, char *locator);

#endif // _FILE_HPP_