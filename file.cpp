#include "file.hpp"
#include "world.hpp"
#include "creature.hpp"
#include "trait.hpp"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int read_cfg(World *world)
{
    int ret = SUCCESS;
    FILE *f = NULL;
    fpos_t cfg_start;
    fpos_t cfg_end;

    int temp_energy = 0;
    char *temp_creature_name = NULL;
    char *temp_trait_name = NULL;
    bool temp_evolve = 0;

    MEM(temp_creature_name, 50, char);
    MEM(temp_trait_name, 50, char);

    VALID(f = fopen("config.cfg","r"), FILE_CODE, FILE_OPEN);

    printf("Finding event\n");
    find_event(f, &cfg_start, CONFIG_START);
    printf("Found event\n");

    HANDLE(fscanf(f, "%d\n", &world->x_size) < 0, FILE_READ);
    HANDLE(fscanf(f, "%d\n", &world->y_size) < 0, FILE_READ);
    HANDLE(fscanf(f, "%d\n", &world->age) < 0, FILE_READ);

    while (fscanf(f, "}\n%s {\n", temp_creature_name) != 0) {
        HANDLE(fscanf(f, "%d\n", &temp_energy) < 0, FILE_READ);
        HANDLE(fscanf(f, "%d\n", &temp_evolve) < 0, FILE_READ);
        Creature temp_creature(temp_energy, temp_creature_name, temp_evolve);
        while(fscanf(f, "%s {\n", temp_trait_name) != 0) {
            Trait temp_trait(temp_trait_name);
            temp_creature.add_trait(temp_trait);
            temp_creature.n_traits++;
            printf("Found trait\n");
        }
        temp_creature.generate_tag();
        world->populate(temp_creature);
        printf("Found creature\n");
    }

exit:
    return ret;
}

int find_event(FILE *fd, fpos_t *prof_start, char *locator)
{
  int ret = SUCCESS;
  int flag = 0;
  int found = 0;
  char *buffer = NULL;

  MEM(buffer, strlen(locator), char);

// Prepares check based on user input profile selection
// Iterates through file and exits when it has found the start of the profile
  do {
    buffer[flag] = fgetc(fd);
    if (buffer[flag] == locator[flag]) {
      if (flag == strlen(locator)-1) {
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
  SFREE(buffer);
  return ret;
}