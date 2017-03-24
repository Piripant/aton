# Aton
Aton is a cross-platform (Windows, Linux, MacOSX) cellular automaton and finite grid simulator written in modern c++.

## Features
* Graphic rapresentation of the world
* Save/load world files functionality
* GUI to edit worlds files and run simulations
* Automaton description via lua scripts
* Simple automaton description format
* Grid scripts to use Aton as a library
* 10 already included automatons
* 1 already included grid script

## Usage
To compile, just write

```
cmake .
make
```

You will find the executable in the bin folder

```
cd bin
./Aton
```

## Dependencies
* GLEW
* SDL 2
* Lua 5.3 (already included)
* ImGui (already included)

## Scripting
You can program two kind of scripts in Aton: automatons and grids.

To describe the type of script in the .lua file, a variable named `SCRIPT_TYPE` is used:

```lua
SCRIPT_TYPE = "CA" -- For cellular automatas
SCRIPT_TYPE = "GRID" -- For grids
```

### Automatons

An automaton is described in lua script which needs to have the following variables and functions:

```lua
SCRIPT_TYPE = "CA" -- This script is defining a cellular automaton
NEIGHBORS_POS -- Array of array of integers like { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }
STATES_NUM -- Integer
function get_next_state (neighbors, state) -- Needs to return an integer
```

* `NEIGHBORS_POS` describes the neighbors positions relative to a cell. That in the example is a von Neumann neighborhood.

* `STATES_NUM` describes the number of states in your automaton.

* ```get_next_state(neighbors, state)``` is the function which describes your automaton behaviour:
The `neighbors` parameter is an array formed of states of the current neighbors, put in the order described by `NEIGHBORS_POS`.
The `state` parameter is the current cell state.
The function must return the output state for the combination of neighbors' and current cell's states as an integer.

Here is an example of a script implementing Conway's game of life:
```lua
SCRIPT_TYPE = "CA"
NEIGHBORS_POS = { {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1} }
STATES_NUM = 2

alive = 1
dead = 0

function get_next_state (neighbors, state)
    local alive_neigh = 0
    for i, neighbor in pairs(neighbors) do
        -- The cell is alive
        if neighbor == alive then
            alive_neigh = alive_neigh + 1
        end
    end

    if state == alive then
        if alive_neigh < 2 then
            return dead
        elseif alive_neigh == 2 or alive_neigh == 3 then
            return alive
        elseif alive_neigh > 3 then
            return dead
        end
    elseif state == dead then
        if alive_neigh == 3 then
            return alive
        end
    end
end
```

### Grids

Grids scripts use Aton almost as a library, and can be used to program anything that needs a 2D grid.

The script needs only the following function and variable:

```lua
SCRIPT_TYPE = "GRID" -- This is a grid script
function step_world() -- A function called to run a step of the simulation
```

Inside `step_world` you can use multiple functions to modify a cell current a future state:

```lua
grids.set_current(x, y, state) -- Sets the cell on the current grid at position x, y to state
grids.set_future(x, y, state) -- Sets the cell on the future grid at position x, y to state
grids.get_current(x, y) -- Returns the cell state on position x, y in the current grid
grids.get_future(x, y) -- Return the cell state on position x, y in the future grid
grids.get_height() -- Returns the grids height
grids.get_width() -- Returns the grids width
grids.swap() -- Swaps the current and future tables
```

Here is an example of a grid script that draws diagonal strapes:

```lua
SCRIPT_TYPE = "GRID"

x = 0
y = 0

function world_step ()
    grids.set_current(x, y, 1)
    x = (x + 1) % grids.get_width()
    y = (y + 1) % grids.get_height()
end
```