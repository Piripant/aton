# Aton
Aton is a cross-platform (Windows, Linux, MacOSX) cellular automaton simulator written in modern c++.

## Features
* Multithreaded simulations
* Graphic rapresentation of the world
* Save/load world files functionality
* GUI to edit worlds files and run simulations
* Automaton description via lua scripts
* Easy automaton description format
* 8 already included automatons

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
If you wish to program your own automatons, you'll find below everything you need to get started.

A automaton is described in lua script which needs to have the following variables and functions:

```lua
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