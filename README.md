# The Enlightened Path

## Short description

This is a simple maze game written in C++ using SFML. A maze is generated with a depth-first backtracker algorithm; the player explores the maze, finds riddles, encounters enemies, and increases a "vision" radius when riddles are solved. The project requires a C++17 compiler and SFML.

## Prerequisites

- A C++17-capable compiler (`g++`, MSVC, clang)
- SFML (graphics, window, system)
- A TrueType font (`.ttf`) placed in `./fonts/` (recommended)

## Quick build (MinGW / g++)

Open PowerShell in the project folder and run (adjust SFML paths for your system):

```powershell
cd "c:\Users\hijaz tr\OneDrive\Documents\GitHub\The-Enlightened-Path"
g++ -std=c++17 -I"C:\SFML\include" src/main.cpp src/Game.cpp src/Maze.cpp src/Player.cpp src/Riddle.cpp src/GameObject.cpp src/Cell.cpp src/Enemy.cpp -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system -o only_maze.exe
# Copy needed SFML DLLs from C:\SFML\bin to the executable folder when using dynamic linking.
```

## Using Visual Studio (MSVC)

- Create an empty project and add the `.cpp` and `.h` files from the `src/` folder.
- Configure include / library directories for SFML (or use `vcpkg` to install SFML).
- Link `sfml-graphics.lib`, `sfml-window.lib`, `sfml-system.lib` (or their static equivalents).

## Run

- Ensure a TTF font exists in `./fonts/` (e.g. `fonts/DejaVuSans.ttf`). The game tries several locations; adding a `fonts` folder is easiest.
- Run the produced executable (e.g. `./only_maze.exe`) from PowerShell.

## Controls

- **Move**: `W`/`A`/`S`/`D` or arrow keys
- **Start/New Game**: `SPACE`
- **Leaderboard**: `L`
- **Submit riddle answer**: `ENTER` (type while riddle is active)
- **Close riddle**: `ESC`
- **Give Up (in-game)**: `G`

## Files of interest

All source files are located in the `src/` folder.

### Core Game Files
- `src/main.cpp` — Program entry point; constructs `Game` and calls `run()`
- `src/Game.h` / `src/Game.cpp` — Main game state machine, rendering loop, input handling, leaderboard management, and overall game orchestration

### Maze & World
- `src/Maze.h` / `src/Maze.cpp` — Maze data structure, recursive backtracker generator algorithm, and maze rendering
- `src/Cell.h` / `src/Cell.cpp` — Individual maze cell representation, wall data, and cell rendering logic

### Player & Movement
- `src/Player.h` / `src/Player.cpp` — Player position, movement mechanics, vision radius, and player rendering

### Enemies & AI
- `src/Enemy.h` / `src/Enemy.cpp` — Enemy entity class with AI pathfinding logic, collision detection, and enemy rendering. Enemies patrol the maze autonomously and pursue the player when detected within their vision range. Includes state management for different enemy behaviors (idle, patrolling, chasing).

### Gameplay Mechanics
- `src/Riddle.h` / `src/Riddle.cpp` — Riddles and riddle marker rendering within the maze
- `src/GameObject.h` / `src/GameObject.cpp` — Base classes for in-world objects providing shared functionality for game entities

## Notes for customization

- **Maze size / cell size**: Constants in `src/Game.h` (e.g. `COLS`, `ROWS`, `CELL_SIZE`). Adjust with care.
- **Enemy behavior**: Modify enemy speed, vision range, and pathfinding logic in `src/Enemy.cpp`
- **Font**: Place a `.ttf` in `./fonts/` or change the font path list in `Game::Game()` within `src/Game.cpp`
- **Leaderboard file**: `leaderboard.txt` in the working directory


