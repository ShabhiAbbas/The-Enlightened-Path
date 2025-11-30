#include "Maze.h"
#include <cstdlib>

// Maze implementation
// This file contains the grid of Cell objects and the recursive backtracker
// maze generator implemented as repeated `step()` calls. The generator
// removes walls between neighboring cells to create a perfect maze.

Maze::Maze(int cols_, int rows_, int cellSize_) : cols(cols_), rows(rows_), cellSize(cellSize_), generating(true), startX(0), startY(0), finishX(cols_ - 1), finishY(rows_ - 1) {
    grid.resize(rows);
    for(int y = 0; y < rows; ++y) {
        grid[y].resize(cols);
        for(int x = 0; x < cols; ++x) {
            grid[y][x] = Cell(x, y);
        }
    }
    current = &grid[0][0];
    current->setVisited(true);
}

Cell* Maze::getCell(int x, int y) {
    if(x < 0 || x >= cols || y < 0 || y >= rows) return nullptr;
    return &grid[y][x];
}

Cell* Maze::getUnvisitedNeighbor(Cell* cell) {
    // Collect unvisited neighbors (top=0, right=1, bottom=2, left=3)
    std::vector<Cell*> neighbors;
    std::vector<int> directions;
    int x = cell->getX();
    int y = cell->getY();
    Cell* top = getCell(x, y - 1);
    Cell* right = getCell(x + 1, y);
    Cell* bottom = getCell(x, y + 1);
    Cell* left = getCell(x - 1, y);
    if(top && !top->isVisited()) { neighbors.push_back(top); directions.push_back(0); }
    if(right && !right->isVisited()) { neighbors.push_back(right); directions.push_back(1); }
    if(bottom && !bottom->isVisited()) { neighbors.push_back(bottom); directions.push_back(2); }
    if(left && !left->isVisited()) { neighbors.push_back(left); directions.push_back(3); }
    if(neighbors.empty()) return nullptr;

    // Randomly choose one neighbor, remove the wall between current and chosen
    int idx = rand() % neighbors.size();
    int dir = directions[idx];
    current->removeWall(dir);
    neighbors[idx]->removeWall((dir + 2) % 4); // remove opposite wall on neighbor
    return neighbors[idx];
}

void Maze::step() {
    // Advance the generator by one step. This is called repeatedly while
    // the game is in the GENERATING state so the UI can show progress.
    if(!generating) return;
    Cell* next = getUnvisitedNeighbor(current);
    if(next) {
        // Move to neighbor: mark visited and push previous cell on stack
        next->setVisited(true);
        stack.push(current);
        current = next;
    } else if(!stack.empty()) {
        // Backtrack: pop previous cell
        current = stack.top();
        stack.pop();
    } else {
        // Completed generation
        generating = false;
    }
}

bool Maze::canMove(int x, int y, int dx, int dy) const {
    if(x + dx < 0 || x + dx >= cols || y + dy < 0 || y + dy >= rows) return false;
    const Cell& cell = grid[y][x];
    if(dx == 0 && dy == -1) return !cell.hasWall(0);
    if(dx == 1 && dy == 0) return !cell.hasWall(1);
    if(dx == 0 && dy == 1) return !cell.hasWall(2);
    if(dx == -1 && dy == 0) return !cell.hasWall(3);
    return false;
}

void Maze::draw(sf::RenderWindow& window) {
    for(int y = 0; y < rows; ++y) {
        for(int x = 0; x < cols; ++x) {
            bool isStart = (x == startX && y == startY);
            bool isFinish = (x == finishX && y == finishY);
            grid[y][x].draw(window, cellSize, isStart, isFinish);
        }
    }
    if(generating && current) {
        sf::RectangleShape highlight(sf::Vector2f(cellSize - 4, cellSize - 4));
        highlight.setPosition(current->getX() * cellSize + 2, current->getY() * cellSize + 2);
        highlight.setFillColor(sf::Color(100,200,255));
        window.draw(highlight);
    }
}

void Maze::drawWithVision(sf::RenderWindow& window, const Player& player) {
    for(int y = 0; y < rows; ++y) {
        for(int x = 0; x < cols; ++x) {
            if(player.isInVision(x, y)) {
                bool isStart = (x == startX && y == startY);
                bool isFinish = (x == finishX && y == finishY);
                grid[y][x].draw(window, cellSize, isStart, isFinish);
            }
        }
    }
}
