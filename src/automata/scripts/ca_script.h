#pragma once
#include <automata/scripts/script.h>
#include <stdint.h>

class CAScript : public Script {
	public:
		~CAScript();
		void Step();
		void LoadScript();
	private:
		uint8_t *rules;
		unsigned int rules_num;
		unsigned int neigh_comb_num;
		unsigned int states_num;
		
		int (*neighbors)[2];
		unsigned int neighbors_num;

		void SetNeighbors();
		void SetRules();
};