#include "Cell.h"


Cell::Cell() : x(0), y(0), visited(false) { for(int i = 0; i < 4; ++i) walls[i] = true;}
Cell::Cell(int x_, int y_) : x(x_), y(y_), visited(false) { for(int i = 0; i < 4; ++i) walls[i] = true;}

void Cell::draw(sf::RenderWindow& window, int cellSize, bool isStart, bool isFinish) const {
    int px = x * cellSize;
    int py = y * cellSize;
    if (visited) {
        sf::RectangleShape bg(sf::Vector2f(cellSize, cellSize));
        bg.setPosition(px, py);
        if (isStart) bg.setFillColor(sf::Color(50, 150, 50, 180));
        else if (isFinish) bg.setFillColor(sf::Color(150, 50, 50, 180));
        else bg.setFillColor(sf::Color(40, 40, 60));
        window.draw(bg);
    }

    sf::Color wallColor(200, 200, 220);
    if (walls[0]) { 
        sf::RectangleShape wall(sf::Vector2f(cellSize, 2)); 
        wall.setPosition(px, py); 
        wall.setFillColor(wallColor); 
        window.draw(wall); 
    }
    if (walls[1]) { 
        sf::RectangleShape wall(sf::Vector2f(2, cellSize)); 
        wall.setPosition(px + cellSize, py); 
        wall.setFillColor(wallColor); 
        window.draw(wall);
    }
    if (walls[2]) { 
        sf::RectangleShape wall(sf::Vector2f(cellSize, 2)); 
        wall.setPosition(px, py + cellSize); 
        wall.setFillColor(wallColor); 
        window.draw(wall); 
    }
    if (walls[3]) { 
        sf::RectangleShape wall(sf::Vector2f(2, cellSize)); 
        wall.setPosition(px, py); 
        wall.setFillColor(wallColor); 
        window.draw(wall); 
    }
}
