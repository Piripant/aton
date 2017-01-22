# Aton
Aton is a cross-platform (Windows, Linux, MacOSX) cellular automaton simulator written in modern c++.

## Features
* Console interface
* Graphic rapresentation of the world
* Console rapresentation of the world
* Save/load world files functionality
* Automaton description via lua scripts

## Usage
To compile, just write

```
cmake .
```

You will find the executable in the bin folder

```
cd bin
./Aton
```

To get a full list of the commands type `help`

To get information on a single command type `cmdhelp {command_name}`

To load a script type `loadscript {path_to_script}`

## Dependencies
* SDL 2
* Lua 5.3 (already included)
