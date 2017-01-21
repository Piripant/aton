#include "world_io.h"
#include <iostream>
#include <fstream>

using namespace std;

int buff_to_integer(char *buffer, unsigned int offset)
{
    return *(int*)(buffer+offset);
    return static_cast<int>(static_cast<unsigned char>(buffer[offset+3]) << 24 |
                            static_cast<unsigned char>(buffer[offset+2]) << 16 | 
                            static_cast<unsigned char>(buffer[offset+1]) << 8 | 
                            static_cast<unsigned char>(buffer[offset+0]));
}

void save_world(World& world, std::string file_name) {
    ofstream save_file;
    Table& table = world.GetCurrentTable();
    
    save_file.open(file_name + ".atonw", ios::binary);

    save_file.write((const char*)&table.width, sizeof(table.width));
    save_file.write((const char*)&table.height, sizeof(table.height));

    for (unsigned int x = 0; x < table.width; x++) {
        for (unsigned int y = 0; y < table.height; y++) {
            uint8_t state = table.GetCellState(x, y);
            save_file.write((const char*)&state, sizeof(state));
        }
    }
    
    for (unsigned int i = 0; i < 256; i++) {
        uint8_t r = world.colors[i][0];
        uint8_t g = world.colors[i][1];
        uint8_t b = world.colors[i][2];

        save_file.write((const char*)&r, sizeof(r));
        save_file.write((const char*)&g, sizeof(g));
        save_file.write((const char*)&b, sizeof(b));
    }
    
    save_file.close();
}

void load_world(World& world, std::string file_name) {
    ifstream file;
    file.open(file_name + ".atonw", ios::binary);

    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    char *buffer = new char[length];
    file.read(buffer, length);
    file.close();

    int width = buff_to_integer(buffer, 0);
    int height = buff_to_integer(buffer, 4);
    int world_len = width*height;

    world.Resize(0, width, height);
    Table& table = world.GetCurrentTable();

    unsigned int offset = 8;
    for (unsigned int i = offset; i < world_len+offset; i++) {
        table.GetCellState(i-8) = static_cast<uint8_t>(buffer[i]);
    }

    offset += world_len;
    for (unsigned int i = offset; i < 256*3+offset; i += 3) {
        world.colors[i-offset][0] = static_cast<uint8_t>(buffer[i]);
        world.colors[i-offset][1] = static_cast<uint8_t>(buffer[i+1]);
        world.colors[i-offset][2] = static_cast<uint8_t>(buffer[i+2]);
    }
    
    delete buffer;
}