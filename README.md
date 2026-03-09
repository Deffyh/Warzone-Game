# Warzone Game

## Overview
This project is a C++ implementation of the core mechanics of the **Warzone strategy board game**, developed for **COMP345**.  
The system models players, maps, territories, orders, and cards using **object-oriented programming and modular design**.

## Features
- **Game Engine** – Controls the main game loop and state transitions.
- **Command Processor** – Handles and validates user commands.
- **Map System** – Loads and validates map files used during gameplay.
- **Player System** – Manages players, territories, and issued orders.
- **Orders Module** – Implements different types of orders available to players.
- **Deck & Cards** – Provides card drawing and card play mechanics.
- **Player Strategies** – Implements AI player behavior using the Strategy design pattern.
- **Logging Observer** – Logs game events using the Observer design pattern.
- **Tournament Mode** – Supports automated gameplay across multiple maps and strategies.

## Project Structure
```
Project
├── CommandProcessor/
├── Engine/
├── Logging-Observer/
├── Orders/
├── Part-4-Deck/
├── Player/
├── PlayerStrategy/
├── part1-map/
│ └── Map.cpp
│
├── Antarctica.map
├── MainDriver.cpp
├── gamelog.txt
├── .gitignore
└── README.md
```

## Requirements
- GCC / G++ compiler
- C++11 or higher

## Compilation

Compile the project using:

```bash
g++ Engine/GameEngine.cpp \
CommandProcessor/CommandProcessor.cpp \
Orders/Orders.cpp \
Part-4-Deck/Cards.cpp \
part1-map/Map.cpp \
Player/Player.cpp \
PlayerStrategy/PlayerStrategy.cpp \
MainDriver.cpp \
-std=c++11
```
Running the Project
After compilation, run the game with:
```
./a.out
```
