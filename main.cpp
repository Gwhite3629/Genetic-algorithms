#include "world.hpp"
#include "file.hpp"

int main(void)
{
    World world(0,0);

    read_cfg(&world);

    return 0;
}
