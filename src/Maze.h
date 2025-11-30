#ifndef MAZE_H
#define MAZE_H

#include "Cell.h"
#include "Player.h"
#include <vector>
#include <stack>
#include <SFML/Graphics.hpp>

class Maze {
private:
    int cols, rows;
    int cellSize;
    std::vector<std::vector<Cell>> grid;
    std::stack<Cell*> stack;
    Cell* current;
    bool generating;
    int startX, startY;
    int finishX, finishY;

    Cell* getCell(int x, int y);
    Cell* getUnvisitedNeighbor(Cell* cell);

public:
    Maze(int cols_, int rows_, int cellSize_);
    ~Maze() {}

    int getStartX() const { return startX; }
    int getStartY() const { return startY; }
    int getFinishX() const { return finishX; }
    int getFinishY() const { return finishY; }
    int getCols() const { return cols; }
    int getRows() const { return rows; }

    void step();
    bool isGenerating() const { return generating; }
    bool canMove(int x, int y, int dx, int dy) const;
    void draw(sf::RenderWindow& window);
    void drawWithVision(sf::RenderWindow& window, const Player& player);
};

#endif // MAZE_H
