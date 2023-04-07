#include "world.h"
#include "file.h"

int main(void)
{
    World world;
    new_world(&world, 0, 0);

    read_cfg(&world);

    for (int i = 0; i < world.n_creatures; i++) {
        printf("Creature: %s\n", ((Creature *)world.creatures[i].data)->name);
        for (int j = 0; j < ((Creature *)world.creatures[i].data)->n_traits; j++) {
            printf("Trait: %s\n", ((Creature *)world.creatures[i].data)->traits[j]->name);
        }
    }

    printf("%s\n", ((Creature *)world.creatures[0].data)->tag_str);

    destroy_world(&world);

    printf("World destroyed\n");
    
    return 0;
}
