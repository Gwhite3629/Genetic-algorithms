#include "world.h"
#include "file.h"

int main(void)
{
    int ret = SUCCESS;
    init;
    World world;
    new_world(&world, 0, 0);

    read_cfg(&world);

    for (int i = 0; i < world.n_creatures; i++) {
        printf("Creature: %s\n", ((Creature *)world.creatures[i].data)->name);
        for (int j = 0; j < ((Creature *)world.creatures[i].data)->n_traits; j++) {
            printf("\tTrait: %s\n", ((Creature *)world.creatures[i].data)->traits[j]->name);
            for (int k = 0; k < ((Creature *)world.creatures[i].data)->traits[j]->n_params; k++) {
                printf("\t\tParam: %d\n", ((Creature *)world.creatures[i].data)->traits[j]->params[k]);
            }
        }
    }

    printf("%s\n", ((Creature *)world.creatures[0].data)->tag_str);

    destroy_world(&world);

    printf("World destroyed\n");

    cleanup();

    end;
exit:
    return ret;
}
