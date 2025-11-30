# The Enlightened Path

A C++ maze exploration game with riddles, built using SFML. Navigate the maze, solve riddles, and increase your vision to find the exit!

---

## Files in This Folder

* `game.cpp` – The main C++ source code.
* `game.exe` – Precompiled executable (Windows).
* `leaderboard.txt` – Saves the top 10 completion times.
* `maze.png` – Maze background or assets.
* `README.md` – Project information.
* 11 `.dll` files – Required SFML dynamic libraries for Windows.
* `.gitignore` – Git ignore file.

---

## How to Run

1. Ensure all `.dll` files are in the same folder as `game.exe`.
2. Double-click `game.exe` to start the game.
3. Controls:

   * **W / Up Arrow:** Move Up
   * **S / Down Arrow:** Move Down
   * **A / Left Arrow:** Move Left
   * **D / Right Arrow:** Move Right
   * **ENTER:** Submit riddle answer
   * **ESC:** Exit or close riddle
   * **G:** Give up / trigger game over
   * **SPACE:** Start new game or replay

---

## Notes for GitHub

* **Do not** upload `game.exe` or `.dll` files to GitHub. They are binary files and can be downloaded separately if needed.
* Only include source code (`.cpp`, `.h`), assets (`.png`, `.ttf`), and configuration files (`.txt`, `.md`, `.gitignore`).
* Add the following to `.gitignore` to exclude binaries:

```
*.exe
*.dll
*.o
*.obj
*.pdb
*.log
```

* Keep `leaderboard.txt` optional if you want a clean repo without user data.

---

## License

Open-source. You can fork and modify it as needed.

---

## Acknowledgements

* [SFML](https://www.sfml-dev.org/) for graphics, window, and system handling.
* Inspired by classic maze and puzzle games.
