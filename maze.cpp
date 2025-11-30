#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <ctime>
#include <cstdlib>

class Cell {
private:
    int x, y;
    bool visited;
    bool walls[4]; // top, right, bottom, left
    
public:
    Cell() : x(0), y(0), visited(false) {
        for(int i = 0; i < 4; i++) walls[i] = true;
    }
    
    Cell(int x, int y) : x(x), y(y), visited(false) {
        for(int i = 0; i < 4; i++) walls[i] = true;
    }
    
    void setVisited(bool v) { visited = v; }
    bool isVisited() const { return visited; }
    
    void removeWall(int direction) { walls[direction] = false; }
    bool hasWall(int direction) const { return walls[direction]; }
    
    int getX() const { return x; }
    int getY() const { return y; }
    
    void draw(sf::RenderWindow& window, int cellSize) const {
        int px = x * cellSize;
        int py = y * cellSize;
        
        // Draw visited cell background
        if(visited) {
            sf::RectangleShape bg(sf::Vector2f(cellSize, cellSize));
            bg.setPosition(px, py);
            bg.setFillColor(sf::Color(40, 40, 60));
            window.draw(bg);
        }
        
        // Draw walls
        sf::Color wallColor(200, 200, 220);
        
        if(walls[0]) { // top
            sf::RectangleShape wall(sf::Vector2f(cellSize, 2));
            wall.setPosition(px, py);
            wall.setFillColor(wallColor);
            window.draw(wall);
        }
        if(walls[1]) { // right
            sf::RectangleShape wall(sf::Vector2f(2, cellSize));
            wall.setPosition(px + cellSize, py);
            wall.setFillColor(wallColor);
            window.draw(wall);
        }
        if(walls[2]) { // bottom
            sf::RectangleShape wall(sf::Vector2f(cellSize, 2));
            wall.setPosition(px, py + cellSize);
            wall.setFillColor(wallColor);
            window.draw(wall);
        }
        if(walls[3]) { // left
            sf::RectangleShape wall(sf::Vector2f(2, cellSize));
            wall.setPosition(px, py);
            wall.setFillColor(wallColor);
            window.draw(wall);
        }
    }
};

class Maze {
private:
    int cols, rows;
    int cellSize;
    std::vector<std::vector<Cell>> grid;
    std::stack<Cell*> stack;
    Cell* current;
    bool generating;
    
    Cell* getCell(int x, int y) {
        if(x < 0 || x >= cols || y < 0 || y >= rows) return nullptr;
        return &grid[y][x];
    }
    
    Cell* getUnvisitedNeighbor(Cell* cell) {
        std::vector<Cell*> neighbors;
        std::vector<int> directions;
        
        int x = cell->getX();
        int y = cell->getY();
        
        Cell* top = getCell(x, y - 1);
        Cell* right = getCell(x + 1, y);
        Cell* bottom = getCell(x, y + 1);
        Cell* left = getCell(x - 1, y);
        
        if(top && !top->isVisited()) {
            neighbors.push_back(top);
            directions.push_back(0);
        }
        if(right && !right->isVisited()) {
            neighbors.push_back(right);
            directions.push_back(1);
        }
        if(bottom && !bottom->isVisited()) {
            neighbors.push_back(bottom);
            directions.push_back(2);
        }
        if(left && !left->isVisited()) {
            neighbors.push_back(left);
            directions.push_back(3);
        }
        
        if(neighbors.empty()) return nullptr;
        
        int idx = rand() % neighbors.size();
        int dir = directions[idx];
        
        // Remove walls between current and chosen neighbor
        current->removeWall(dir);
        neighbors[idx]->removeWall((dir + 2) % 4); // opposite direction
        
        return neighbors[idx];
    }
    
public:
    Maze(int cols, int rows, int cellSize) 
        : cols(cols), rows(rows), cellSize(cellSize), generating(true) {
        
        // Initialize grid
        grid.resize(rows);
        for(int y = 0; y < rows; y++) {
            grid[y].resize(cols);
            for(int x = 0; x < cols; x++) {
                grid[y][x] = Cell(x, y);
            }
        }
        
        // Start from top-left
        current = &grid[0][0];
        current->setVisited(true);
    }
    
    void step() {
        if(!generating) return;
        
        Cell* next = getUnvisitedNeighbor(current);
        
        if(next) {
            next->setVisited(true);
            stack.push(current);
            current = next;
        } else if(!stack.empty()) {
            current = stack.top();
            stack.pop();
        } else {
            generating = false;
        }
    }
    
    bool isGenerating() const { return generating; }
    
    void draw(sf::RenderWindow& window) {
        for(int y = 0; y < rows; y++) {
            for(int x = 0; x < cols; x++) {
                grid[y][x].draw(window, cellSize);
            }
        }
        
        // Highlight current cell during generation
        if(generating && current) {
            sf::RectangleShape highlight(sf::Vector2f(cellSize - 4, cellSize - 4));
            highlight.setPosition(current->getX() * cellSize + 2, 
                                 current->getY() * cellSize + 2);
            highlight.setFillColor(sf::Color(100, 200, 255));
            window.draw(highlight);
        }
    }
};

int main() {
    srand(time(0));
    
    const int CELL_SIZE = 30;
    const int COLS = 25;
    const int ROWS = 20;
    
    sf::RenderWindow window(sf::VideoMode(COLS * CELL_SIZE, ROWS * CELL_SIZE), 
                           "Recursive Backtracking Maze Generator");
    window.setFramerateLimit(60);
    
    Maze maze(COLS, ROWS, CELL_SIZE);
    
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
            
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Space && !maze.isGenerating()) {
                    maze = Maze(COLS, ROWS, CELL_SIZE); // Reset maze
                }
            }
        }
        
        // Generate maze step by step
        if(maze.isGenerating()) {
            for(int i = 0; i < 3; i++) { // 3 steps per frame for faster generation
                maze.step();
            }
        }
        
        window.clear(sf::Color(20, 20, 30));
        maze.draw(window);
        window.display();
    }
    
    return 0;
}