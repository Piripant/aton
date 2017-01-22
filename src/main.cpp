#include <stdint.h>
#include "automata/world.h"
#include "graphics/graphics.h"
#include "graphics/console_ui.h"
#include <iostream>
#include <thread>

int main() {
	World world = World(1, 0, 10, 10);

	std::thread console_t(console_ui_loop, std::ref(world));

	draw_world(world);

	return 0;
}