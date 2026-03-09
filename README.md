# Warzone Game

## Overview
This project is a C++ implementation of the core mechanics of the Warzone strategy board game, developed for COMP345.

The current version is command-driven and runs in manual mode from the console. Players progress through startup states (map load, validation, player setup, game start) and then enter the main game loop phases.

## Current Features
- Game engine state machine and main game loop
- Manual command input via `CommandProcessor`
- Map loading and validation
- Player creation and setup (all players are human/manual)
- Orders issuing and execution
- Deck and cards support
- Logging via observer pattern

## Supported Startup Commands
Use these commands during startup:
- `loadmap <filename>`
- `validatemap`
- `addplayer <name>`
- `gamestart`
- `exit`

After a win state:
- `replay`
- `quit`

## Project Structure
```text
Warzone-Game1/
├── CommandProcessor/
├── Engine/
├── Logging-Observer/
├── Orders/
├── Part-4-Deck/
├── Player/
├── part1-map/
├── MainDriver.cpp
├── CMakeLists.txt
└── README.md
```

## Build Requirements
- CMake 4.0+
- A C++17-compatible compiler

## Build And Run (CMake)
```bash
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug
```

Run the executable:

On Windows:
```bash
cmake-build-debug/Warzone_Game1.exe
```

On Linux/macOS:
```bash
./cmake-build-debug/Warzone_Game1
```

## Notes
- Tournament mode was removed in this version.
- Player strategy mode was removed; no AI strategy players are used.
- Startup and gameplay input are manual through the console.
