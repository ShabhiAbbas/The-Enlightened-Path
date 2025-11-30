# Maze Generation using Recursive Backtracking (C++ & SFML)

This repository contains a C++ implementation of a 2D maze generator using the **recursive backtracking algorithm**, built with **SFML 2.5.x** for rendering.

This code is part of the development process for our larger OOP project *“The Enlightened Path”*, where this maze will later be integrated into a complete game environment.

---

## 🧩 Features

- 2D maze stored in a grid / 2D array  
- Recursive Backtracking algorithm  
- Randomized maze generation for different layouts every run  
- Uses SFML to visually display the maze  
- Clean C++ implementation  

---

## 📁 Files in This Repository

| File | Description |
|------|-------------|
| `maze.cpp` | The main implementation of the maze generator (SFML + recursive backtracking). |

> **Note:** SFML itself is *not included* in this repository to keep it lightweight.  
You must install SFML separately (instructions below).

---

## 🔧 How to Compile (Windows, using g++)

This program uses **SFML 2.5.x**, so make sure SFML is installed.

### **1. Download SFML**
Download from the official website:  
https://www.sfml-dev.org/download.php

Extract the SFML folder anywhere on your system.

---

### **2. Copy SFML DLLs**
Inside the SFML folder:

1. Go to: `SFML/bin/`  
2. Copy all `.dll` files  
3. Paste them into the same folder where your compiled executable (`.exe`) will be created  

This ensures the program can find SFML libraries at runtime.

---

### **3. Ignore IDE warnings**
If your editor shows a squiggly line under:

```cpp
#include <SFML/Graphics.hpp>
