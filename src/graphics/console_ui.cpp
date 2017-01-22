#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include "../automata/world.h"
#include "console_ui.h"
#include "../io/world_io.h"

using namespace std;

// The commands function take two parameters:
// A reference to the current world (World&)
// The reference to the parameters passed to the command (vector<string>&)
// Note: the first parameter is the command name

#define COMMAND_PARAMETERS World& world, vector<string>& parameters

void print_help(COMMAND_PARAMETERS);
void print_cmd_help(COMMAND_PARAMETERS);
void set_state(COMMAND_PARAMETERS);
void print_state(COMMAND_PARAMETERS);
void print_script_states(COMMAND_PARAMETERS);
void step(COMMAND_PARAMETERS);
void print_world(COMMAND_PARAMETERS);
void print_world_size(COMMAND_PARAMETERS);
void reset_world(COMMAND_PARAMETERS);
void set_state_color(COMMAND_PARAMETERS);
void print_colors(COMMAND_PARAMETERS);
void load_script(COMMAND_PARAMETERS);
void save_world_cmd(COMMAND_PARAMETERS); // The cmd suffix is to not overlap with save_world from world_io.h
void load_world_cmd(COMMAND_PARAMETERS); // The cmd suffix is to not overlap with load_world from world_io.h

// This class holds a command
// The exec_command is a pointer to the function that must be called when the command is called
struct command {
    string name;
    string parameters;
    string description;
    unsigned int parameters_num;
    void (*exec_command)(COMMAND_PARAMETERS);
};

// This array holds all the defined commands of the application
#define COMMANDS_NUM 14
command COMMANDS[COMMANDS_NUM] = {
    command {"help", "takes 0 parameters", "prints all the commands", 0, &print_help},
    command {"cmdhelp", "takes 1 parameter {cmd_name}", "prints the help of the command {cmd_name}", 1, &print_cmd_help},
    command {"setstate", "takes 3 parameters {x} {y} {new_state}", "sets the state of {x} {y} to {new_state}", 3, &set_state},
    command {"prnstate", "takes 2 parameters {x} {y}", "prints the state of the cell at {x}, {y}", 2, &print_state},
    command {"maxscriptstate", "takes 0 parameters", "prints the number of states use by the current script", 0, &print_script_states},
    command {"step", "takes 2 parameters {n} {t}", "simulates {n} steps with a interval of {t} in milliseconds", 2, &step},
    command {"prnworld", "takes 0 parameters", "prints a graphical rapresentation of the world on the terminal", 0, &print_world},
    command {"prnworldsize", "takes 0 parameters", "prints the world size", 0, &print_world_size},
    command {"rstworld", "takes 3 parameters {width} {height} {state}", "sets the world to {width}x{height} and all the cells to {state}", 3, &reset_world},
    command {"setcolor", "takes 4 parameters {state} {r} {g} {b}", "sets the {state} color to {r} {g} {b}", 4, &set_state_color},
    command {"prncolors", "takes 0 parameters", "prints all the states and their corresponding color", 0, &print_colors},
    command {"loadscript", "takes 1 parameter {script_name}", "load the script {script_name}", 1, &load_script},
    command {"saveworld", "takes 1 parameter {file_name}", "saves the world in {file_name}", 1, &save_world_cmd},
    command {"loadworld", "takes 1 parameter {file_name}", "load the world in {file_name}", 1, &load_world_cmd},
};

bool check_num_range(int number, string name, int min_value, int max_value) {    
    if (number < min_value || number > max_value) {
        cout << "The " << name << " must be lower than " << min_value << " and greater than " << max_value << endl;
        return false;
    }

    return true;
}

void print_help(COMMAND_PARAMETERS) {
    cout << endl;
    cout << "Here is a list of all the commands" << endl;
    for (unsigned int i = 0; i < COMMANDS_NUM; i++) {
        cout << "-- " << COMMANDS[i].name << endl;
        cout << "-->> " << COMMANDS[i].parameters << endl;
    }
    cout << endl;
}

void print_cmd_help(COMMAND_PARAMETERS) {
    string cmd_name = parameters[1];
    for (unsigned int i = 0; i < COMMANDS_NUM; i++) {
        if (COMMANDS[i].name == cmd_name) {
            cout << "-->> " << COMMANDS[i].parameters << ": " << COMMANDS[i].description << endl;
            break;
        }
    }
}

void set_state(COMMAND_PARAMETERS) {
    int x = std::stoi(parameters[1]);
    int y = std::stoi(parameters[2]);
    int state = std::stoi(parameters[3]);

    // Check if the state is valid
    if (!check_num_range(state, "state", 0, 255)) {
        return;
    }

    world.GetCurrentTable().GetCellState(x, y) = state;
}

void print_state(COMMAND_PARAMETERS) {
    int x = std::stoi(parameters[1]);
    int y = std::stoi(parameters[2]);

    cout << (unsigned int)world.GetCurrentTable().GetCellState(x, y) << endl;
}

void print_script_states(COMMAND_PARAMETERS) {
    cout << world.script_states-1 << endl;
}

void step_thread(World& world, unsigned int steps, unsigned int sleep, bool& stop) {
    unsigned int hundredth = steps/100;

    unsigned int i;
    for (i = 0; i < steps; i++) {
        if(stop) {
            break;
        }

        world.Step();
        this_thread::sleep_for(chrono::milliseconds(sleep));
    }

    cout << "Finished at step " << i << "/" << steps << ", press enter to continue" << "\r";
}

void step(COMMAND_PARAMETERS) {
    cout << "Press enter to stop or continue\r";
    unsigned int steps = std::stoi(parameters[1]);
    unsigned int sleep = std::stoi(parameters[2]);
    bool stop = false;
    thread step_t(step_thread, std::ref(world), steps, sleep, std::ref(stop));
    
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
    int state = std::stoi(parameters[3]);
    
    // Check if the state is valid
    if (!check_num_range(state, "state", 0, 255)) {
        return;
    }
    
    world.Resize((uint8_t)state, w, h);
}

void set_state_color(COMMAND_PARAMETERS) {
    int args[4] = {stoi(parameters[1]), stoi(parameters[2]), stoi(parameters[3]), stoi(parameters[4])};
    string NAMES[4] = {"state", "r", "g", "b"}; 
    for (unsigned int i = 0; i < 4; i++) {
        if (!check_num_range(args[i], NAMES[i], 0, 255)) {
            return;
        }
    }
    world.colors[args[0]] = {(uint8_t)args[1], (uint8_t)args[2], (uint8_t)args[3]};
}

void print_colors(COMMAND_PARAMETERS) {
    cout << "Default color: black" << endl;
    for (unsigned int i = 0; i < 255; i++) {
        uint8_t r = world.colors[i][0];
        uint8_t g = world.colors[i][1];
        uint8_t b = world.colors[i][2];

        if (r != 0 || g != 0 || b != 0) {
            cout << i << ": " << (int)r << " " << (int)g << " " << (int)b << endl;
        }
    }
}

void load_script(COMMAND_PARAMETERS) {
    world.LoadFromFile((char*)parameters[1].c_str());
}

void save_world_cmd(COMMAND_PARAMETERS) {
    save_world(world, parameters[1]);
}

void load_world_cmd(COMMAND_PARAMETERS) {
    load_world(world, parameters[1]);
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
    cout << "Type help to get a full list of the commands" << endl;
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