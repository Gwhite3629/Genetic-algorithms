#ifndef _FILE_HPP_
#define _FILE_HPP_

#include "world.hpp"

#include <cstdio>

#define CONFIG_START "################\nWORLD_SETTINGS {\n"

int read_cfg(World*);

int find_event(FILE *fd, fpos_t *prof_start, char *locator);

#endif // _FILE_HPP_