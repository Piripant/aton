#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include "../automata/world.h"
#include "console_ui.h"
#include "graphics.h"

using namespace std;

// The commands function take two parameters:
// A reference to the current world (World&)
// The reference to the parameters passed to the command (vector<string>&)
// Note: the first parameter is the command name

#define COMMAND_PARAMETERS World& world, vector<string>& parameters

void print_help(COMMAND_PARAMETERS);
void set_state(COMMAND_PARAMETERS);
void print_state(COMMAND_PARAMETERS);
void print_max_state(COMMAND_PARAMETERS);
void step(COMMAND_PARAMETERS);
void print_world(COMMAND_PARAMETERS);
void print_world_size(COMMAND_PARAMETERS);
void reset_world(COMMAND_PARAMETERS);
// void load_script(COMMAND_PARAMETERS);
// void load_world(COMMAND_PARAMETERS);
void render(COMMAND_PARAMETERS);

// This class holds a command
// The exec_command is a pointer to the function that must be called when the command is called
struct command {
    string name;
    string description;
    unsigned int parameters_num;
    void (*exec_command)(COMMAND_PARAMETERS);
};

// This array holds all the defined commands of the application
#define COMMANDS_NUM 9
command COMMANDS[COMMANDS_NUM] = {
    command {"help", "prints all the commands", 0, &print_help},
    command {"setstate", "takes 3 parameters {x} {y} {new_state}", 3, &set_state},
    command {"printstate", "takes 2 parameters {x} {y} and prints the state of the cell at {x}, {y}", 2, &print_state},
    command {"printmaxstate", "takes 0 parameters and prints the number of states defined", 0, &print_max_state},
    command {"step", "takes 1 parameter {n} and simulates {n} steps", 1, &step},
    command {"printworld", "takes 0 parameters and prints a graphical rapresentation of the world on the terminal", 0, &print_world},
    command {"printworldsize", "takes 0 parameters and prints the world size", 0, &print_world_size},
    command {"resetworld", "takes 3 parameters {width} {height} {state} and sets the world to {width}x{height} and all the cells to {state}", 3, &reset_world},
    command {"render", "takes 0 parameters and renders the world in a SDL2 window", 0, &render}
};

void print_help(COMMAND_PARAMETERS) {
    cout << endl;
    cout << "Here is a list of all the commands" << endl;
    for (unsigned int i = 0; i < COMMANDS_NUM; i++) {
        cout << "-- " << COMMANDS[i].name << endl;
        cout << "-->> " << COMMANDS[i].description << endl;
    }
    cout << endl;
}

void set_state(COMMAND_PARAMETERS) {
    int x = std::stoi(parameters[1]);
    int y = std::stoi(parameters[2]);
    int state = std::stoi(parameters[3]);

    // Check if the state is valid
    if (state < 0 || state >= world.states_num) {
        cout << "State must be between 0 and " << world.states_num-1 << " (the declared max state)" << endl;
        return;
    }

    world.GetCurrentTable().GetCellState(x, y) = state;
}

void print_state(COMMAND_PARAMETERS) {
    int x = std::stoi(parameters[1]);
    int y = std::stoi(parameters[2]);

    cout << (unsigned int)world.GetCurrentTable().GetCellState(x, y) << endl;
}

void print_max_state(COMMAND_PARAMETERS) {
    cout << world.states_num-1 << endl;
}

void step_thread(World& world, unsigned int steps, bool& stop) {
    unsigned int hundredth = steps/100;

    unsigned int i;
    for (i = 0; i < steps; i++) {
        if(!stop) {
            world.Step();
        }
        else {
            break;
        }
    }

    cout << "Finished at step " << i << "/" << steps << ", press enter to continue" << "\r";
}

void step(COMMAND_PARAMETERS) {
    cout << "Press enter to stop or continue\r";
    unsigned int steps = std::stoi(parameters[1]);
    bool stop = false;
    thread step_t(step_thread, std::ref(world), steps, std::ref(stop));
    
    // TODO: don't use the standard input to wait
    // Wait for the input
    string input;
    getline(cin, input);
    stop = true;
    step_t.join();
}

void print_world(COMMAND_PARAMETERS) {
	for (int y = world.GetCurrentTable().height - 1; y >= 0; y--) {
		for (unsigned int x = 0; x < world.GetCurrentTable().width; x++) {
			uint8_t state = world.GetCurrentTable().GetCellState(x, y);
			cout << (unsigned int)state << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void print_world_size(COMMAND_PARAMETERS) {
    cout << world.GetCurrentTable().width << "x" << world.GetCurrentTable().height << endl;
}

void reset_world(COMMAND_PARAMETERS) {
    unsigned int h = std::stoi(parameters[1]);
    unsigned int w = std::stoi(parameters[2]);
    uint8_t state = std::stoi(parameters[3]);

    world.Resize(state, w, h);
}

void render(COMMAND_PARAMETERS) {
    draw_world(world);
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

void console_ui_loop(World& world) {  
    cout << "Welcome to aton, a command line interfaced cellular automata simulator" << endl;
    while (true) {
        // Use just cin to read the input
        string input;
        getline(cin, input);

        if (input == "") {
            continue;
        }

        vector<string> parameters; // The first parameter will be the command name
        split(input, ' ', parameters);

        // Loop through the commands array
        bool command_unknown = true;
        for (unsigned int i = 0; i < COMMANDS_NUM; i++) {
            if (parameters[0] == COMMANDS[i].name) {
                command_unknown = false;
                
                // parameters.size()-1 to cout off the command name
                if (parameters.size()-1 != COMMANDS[i].parameters_num) {
                    cout << "Wrong number of arguments: " << COMMANDS[i].name << " expected " << COMMANDS[i].parameters_num << " arguments" << endl;
                    break;
                }
                COMMANDS[i].exec_command(world, parameters);
            }
        }

        if (command_unknown) {
            cout << parameters[0] << ": command not found" << endl;
        }
    }
}