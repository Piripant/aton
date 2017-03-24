#include <stdint.h>
#include <automata/world.h>
#include <graphics/graphics.h>
#include <iostream>
#include <thread>

int main() {
	World::Initialize(0, 10, 10);
	draw_world();
	return 0;
}