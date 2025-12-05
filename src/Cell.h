#ifndef CELL_H
#define CELL_H

#include <SFML/Graphics.hpp>

class Cell {
private:
    int x, y;
    bool visited;
    bool walls[4]; // top, right, bottom, left

public:
    Cell();
    Cell(int x_, int y_);

    void setVisited(bool v) { visited = v; }
    bool isVisited() const { return visited; }

    void removeWall(int direction) { walls[direction] = false; }
    bool hasWall(int direction) const { return walls[direction]; }

    int getX() const { return x; }
    int getY() const { return y; }

    void draw(sf::RenderWindow& window, int cellSize, bool isStart = false, bool isFinish = false) const;
};

#endif 
