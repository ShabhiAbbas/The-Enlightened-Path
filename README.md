## The Enlightened Path (only_maze)

Short description

This is a simple maze game written in C++ using SFML. A maze is generated with a depth-first backtracker algorithm; the player explores the maze, finds riddles, and increases a "vision" radius when riddles are solved. The project requires a C++17 compiler and SFML.

Prerequisites

- A C++17-capable compiler (`g++`, MSVC, clang)
- SFML (graphics, window, system)
- A TrueType font (`.ttf`) placed in `./fonts/` (recommended)

Quick build (MinGW / g++)

Open PowerShell in the project folder and run (adjust SFML paths for your system):

```powershell
g++ -std=c++17 -I"C:\SFML\include" main.cpp Game.cpp Maze.cpp Player.cpp Riddle.cpp GameObject.cpp Cell.cpp -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system -o only_maze.exe
# Copy needed SFML DLLs from C:\SFML\bin to the executable folder when using dynamic linking.
```

Using Visual Studio (MSVC)

- Create an empty project and add the `.cpp` and `.h` files.
- Configure include / library directories for SFML (or use `vcpkg` to install SFML).
- Link `sfml-graphics.lib`, `sfml-window.lib`, `sfml-system.lib` (or their static equivalents).

Run

- Ensure a TTF font exists in `./fonts/` (e.g. `fonts/DejaVuSans.ttf`). The game tries several locations; adding a `fonts` folder is easiest.
- Run the produced executable (e.g. `./only_maze.exe`) from PowerShell.

Controls

- Move: `W`/`A`/`S`/`D` or arrow keys
- Start/New Game: `SPACE`
- Leaderboard: `L`
- Submit riddle answer: `ENTER` (type while riddle is active)
- Close riddle: `ESC`
- Give Up (in-game): `G`

Files of interest

- `main.cpp` — program entry; constructs `Game` and calls `run()`
- `Game.h` / `Game.cpp` — main game state, rendering, input handling, leaderboard
- `Maze.h` / `Maze.cpp` — maze data, generator (recursive backtracker), drawing
- `Cell.h` / `Cell.cpp` — a single maze cell and its draw logic
- `Player.h` / `Player.cpp` — player position, vision and drawing
- `Riddle.h` / `Riddle.cpp` — riddles and marker rendering
- `GameObject.h` / `GameObject.cpp` — base classes for in-world objects

Notes for customization

- Maze size / cell size: constants in `Game.h` (e.g. `COLS`, `ROWS`, `CELL_SIZE`). Adjust with care.
- Font: place a `.ttf` in `./fonts/` or change the font path list in `Game::Game()` within `Game.cpp`.
- Leaderboard file: `leaderboard.txt` in the working directory.

If you want more detail about the classes and methods, see `METHODS.md` which explains how each class works and the main algorithms used.

