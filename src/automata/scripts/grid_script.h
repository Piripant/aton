#pragma once
#include <automata/scripts/script.h>
#include <stdint.h>

class GridScript : public Script {
	public:
		void Step();
		void LoadScript();
};