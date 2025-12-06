# The Enlightened Path

## Short description

This is a simple maze game written in C++ using SFML. A maze is generated with a depth-first backtracker algorithm; the player explores the maze, finds riddles, encounters enemies, and increases a "vision" radius when riddles are solved. The project requires a C++17 compiler and SFML.

## Prerequisites

- A C++17-capable compiler (`g++`, MSVC, clang)
- SFML (graphics, window, system)
- A TrueType font (`.ttf`) placed in `./fonts/` (recommended)

## Compilation & Running

### Using Visual Studio (MSVC)

1. Create an empty project and add all `.cpp` and `.h` files from the `src/` folder.
2. Configure include and library directories for SFML (or use `vcpkg` to install SFML):
   ```
   vcpkg install sfml:x64-windows
   ```
3. Link the following libraries:
   - `sfml-graphics.lib`
   - `sfml-window.lib`
   - `sfml-system.lib`
   - `sfml-audio.lib` (for music support)
4. Build the project (press `F7` or use **Build > Build Solution**).
5. Run the executable: `./The-Enlightened-Path.exe` from PowerShell in the project directory.

### Using G++ (MinGW/Linux/Mac)

1. Install SFML development libraries:
   - **Windows (MSYS2)**: `pacman -S mingw-w64-x86_64-sfml`
   - **Ubuntu/Debian**: `sudo apt-get install libsfml-dev`
   - **macOS**: `brew install sfml`

2. Compile from the project root directory:
   ```bash
   g++ -std=c++17 -o maze_game src/*.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
   ```

3. Run the game:
   ```bash
   ./maze_game
   ```



### Setup & Prerequisites

- Ensure a TTF font exists in `./fonts/` (e.g., `fonts/DejaVuSans.ttf` or `fonts/arial.ttf`). The game searches multiple locations, but having a `fonts` folder is recommended.
- Place audio file `haunted.wav` in the project root (or modify the path in `src/Game.cpp`).
- The game auto-generates `riddles.txt` if it doesn't exist.
- Leaderboard scores are saved to `leaderboard.txt` in the working directory.

## Controls

- **Move**: `W`/`A`/`S`/`D` or arrow keys
- **Start/New Game**: `SPACE`
- **Leaderboard**: `TAB`
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


