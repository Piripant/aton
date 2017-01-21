#include <string>
#include "../automata/world.h"

// File format description:
// 32 bits for the world width
// 32 bits for the world height
// 8*width*height bits for the cells states
// 8*256 bits for the state colors

void save_world(World& world, std::string file_name);
void load_world(World& world, std::string file_name);