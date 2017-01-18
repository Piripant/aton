#include <vector>
#include "table.h"


Table::Table(uint8_t state, unsigned int width, unsigned int height) {
	this->_cells_states = new uint8_t[width*height];
	this->width = width;
	this->height = height;
	this->len = width*height;

	for (unsigned int i = 0; i < width*height; i++) {
		this->_cells_states[i] = state;
	}
}