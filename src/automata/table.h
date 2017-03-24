#pragma once
#include <stdint.h>

class Table {
	protected:
		uint8_t *_cells_states;
	public:
		unsigned int width;
		unsigned int height;
		unsigned int len;

		Table(uint8_t state = 0, unsigned int width = 1, unsigned int height = 1);
		inline unsigned int Get_x(unsigned int index);
		inline unsigned int Get_y(unsigned int index);
		inline uint8_t& GetCellState(unsigned int x, unsigned int y);
		inline uint8_t& GetCellState(unsigned int index);
		inline bool SafeSetCell(uint8_t state, unsigned int x, unsigned int y); // Return true if the cell is in bounds
		inline bool InBounds(unsigned int x, unsigned int y);
};

inline uint8_t& Table::GetCellState(unsigned int x, unsigned int y) {
	return this->_cells_states[y*width + x];
}

inline uint8_t& Table::GetCellState(unsigned int index) {
	return this->_cells_states[index];
}

inline unsigned int Table::Get_x(unsigned int index) {
	return (index % width + width) % width;
}

inline unsigned int Table::Get_y(unsigned int index) {
	return ((index/width + height) % height) * width;
}

inline bool Table::SafeSetCell(uint8_t state, unsigned int x, unsigned int y) {
	if (x < width && y < height) {
		GetCellState(x, y) = state;
		return true;
	}

	return false;
}

inline bool Table::InBounds(unsigned int x, unsigned int y) {
	if (x < width && y < height) {
		return true;
	}
	else {
		return false;
	}
}