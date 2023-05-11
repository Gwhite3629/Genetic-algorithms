#include "file.h"
#include "world.h"
#include "creature.h"
#include "trait.h"
#include "utils.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int read_cfg(World *world)
{
    int ret = SUCCESS;
    FILE *f = NULL;
    fpos_t cfg_start;

    int temp_energy = 0;
    char *temp_creature_name = NULL;
    char *temp_trait_name = NULL;
    int temp_evolve = 0;
    int temp_ntraits = 0;
    int temp_ncreatures = 0;

    Creature **temp_creature = NULL;

    temp_creature = new(temp_creature, 1, Creature *);

    temp_creature_name = new(temp_creature_name, 50, char);
    temp_trait_name = new(temp_trait_name, 50, char);

    VALID(f = fopen("config.cfg","r"), FILE_CODE, FILE_OPEN);

    find_event(f, &cfg_start, CONFIG_START);

    HANDLE(fscanf(f, "%d\n", &(world->x_size)) < 0, FILE_READ);
    HANDLE(fscanf(f, "%d\n", &(world->y_size)) < 0, FILE_READ);
    HANDLE(fscanf(f, "%d\n", &(world->age)) < 0, FILE_READ);
    HANDLE(fscanf(f, "%d\n", &temp_ncreatures) < 0, FILE_READ);


    while (fscanf(f, "Creature : %s\n", temp_creature_name) == 1) {
        HANDLE(fscanf(f, "%d\n", &temp_energy) < 0, FILE_READ);
        HANDLE(fscanf(f, "%d\n", &temp_evolve) < 0, FILE_READ);
        HANDLE(fscanf(f, "%d\n", &temp_ntraits) < 0, FILE_READ);
        temp_creature[world->n_creatures] = new(temp_creature[world->n_creatures], 1, Creature);
        new_creature(temp_creature[world->n_creatures], temp_energy, temp_creature_name, temp_evolve);
        while(fscanf(f, "Trait : %s\n", temp_trait_name) == 1) {
            add_trait(temp_creature[world->n_creatures], temp_trait_name);
        }
        generate_tag(temp_creature[world->n_creatures]);
        map_insert(&(world->creatures), &(world->n_creatures), temp_creature[world->n_creatures]->tag_str, temp_creature[world->n_creatures]);
        temp_creature = widen(temp_creature, world->n_creatures, Creature *);
    }

exit:
    SCLOSE(f);
    del(temp_creature);
    del(temp_creature_name);
    del(temp_trait_name);
    return ret;
}

int find_event(FILE *fd, fpos_t *prof_start, char *locator)
{
  int ret = SUCCESS;
  int flag = 0;
  int found = 0;
  char *buffer = NULL;

  buffer = new(buffer, strlen(locator), char);

// Prepares check based on user input profile selection
// Iterates through file and exits when it has found the start of the profile
  do {
    buffer[flag] = fgetc(fd);
    if (buffer[flag] == locator[flag]) {
      if (flag == (int)(strlen(locator)-1)) {
        if (found == 0)
          fgetpos(fd, prof_start);
        found++;
        flag=0;
      } else {
        flag++;
      }
    } else {
      flag = 0;
    }
    // if (flag > 0)
      // printf("buffer[flag]: %c, locator[flag]: %c, strlen(locator): %I64d, flag: %d, found: %d\n", buffer[flag], locator[flag], strlen(locator), flag, found);
  } while (found < 1);
exit:
  del(buffer);
  return ret;
}