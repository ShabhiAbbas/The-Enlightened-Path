#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;


enum GameState {
    WELCOME,
    GENERATING,
    PLAYING,
    RIDDLE_ACTIVE,
    GAME_OVER,
    VICTORY,
    LEADERBOARD_VIEW
};


class GameObject {
    protected:
        float x, y;
        
    public:
        GameObject(float posX = 0, float posY = 0) : x(posX), y(posY) {}
        virtual ~GameObject() {}
        
        virtual void draw(sf::RenderWindow& window) const = 0;
        
        virtual void update(float deltaTime) {}
        
        float getX() const { return x; }
        float getY() const { return y; }
        void setPosition(float posX, float posY) { x = posX; y = posY; }
};


class MovableEntity : public GameObject {
    protected:
        int direction; // 0=up, 1=right, 2=down, 3=left
        float speed;
        
    public:
        MovableEntity(float posX = 0, float posY = 0, float speed = 1.0f) :GameObject(posX, posY), direction(2), speed(speed) {}
        
        virtual ~MovableEntity() {}
        
        virtual void move(int xpos, int ypos){
            x += xpos * speed;
            y += ypos * speed;
            
            if(xpos < 0){
                direction = 3;
            } else if(xpos > 0){
                direction = 1;
            }else if(ypos < 0){
                direction = 0;
            }else if(ypos > 0) {
                direction = 2;
            }
        }
        
        int getDirection() const{ 
            return direction; 
        }
};


class Riddle : public GameObject{
    protected:
        string question;
        string answer;
        bool solved;
        int cellSize;

        virtual sf::Color getMarkerColor() const {
            return sf::Color(255, 200, 50);
        }
        
    public:
        Riddle(string q = "", string a = "", int posX = 0, int posY = 0, int cs = 30): GameObject(posX, posY), question(q), answer(a), solved(false), cellSize(cs){}
        
        virtual ~Riddle() {}

        string getQuestion() const { return question; }
        string getAnswer() const { return answer; }
        bool isSolved() const { return solved; }
        void setSolved(bool s) { solved = s; }
        
      
        virtual float getReward() const { return 2.0f; }
    
        virtual void draw(sf::RenderWindow& window) const override {
            if(!solved) {
                float px = x * cellSize + cellSize / 2;
                float py = y * cellSize + cellSize / 2;
                
                sf::CircleShape marker(8, 6);
                marker.setOrigin(8, 8);
                marker.setPosition(px, py);
                marker.setFillColor(getMarkerColor());
                marker.setOutlineColor(sf::Color(255, 255, 100));
                marker.setOutlineThickness(2);
                window.draw(marker);
            }
        }
        
};

class EasyRiddle : public Riddle {
    public:
        EasyRiddle(std::string q, std::string a, int posX, int posY, int cs): Riddle(q, a, posX, posY, cs) {}
        
        virtual float getReward() const override { return 1.5f; }
        
    protected:
        virtual sf::Color getMarkerColor() const override {
            return sf::Color(100, 255, 100); // Green for easy
        }
};


class HardRiddle : public Riddle {
    public:
        HardRiddle(std::string q, std::string a, int posX, int posY, int cs):Riddle(q, a, posX, posY, cs) {}
        
        virtual float getReward() const override { return 3.0f; }
        
    protected:
        virtual sf::Color getMarkerColor() const override {
            return sf::Color(255, 100, 100); // Red for hard
        }
};


struct LeaderboardEntry {
    string name;
    float time;
    
    LeaderboardEntry() : name(""), time(0) {}
    LeaderboardEntry(string n, float t) : name(n), time(t) {}
    
    bool operator<(const LeaderboardEntry& other) const{
        return time < other.time;
    }
};


class Cell {
    private:
        int x, y;
        bool visited;
        bool walls[4]; // top, right, bottom, left
        
    public:
        Cell() : x(0), y(0), visited(false){
            for(int i = 0; i < 4; i++){
                walls[i] = true;
            }
        }
        
        Cell(int x, int y) : x(x), y(y), visited(false) {
            for(int i = 0; i < 4; i++){
                walls[i] = true;
            }
        }
        
        void setVisited(bool v) { visited = v; }
        bool isVisited() const { return visited; }
        
        void removeWall(int direction) { walls[direction] = false; }
        bool hasWall(int direction) const { return walls[direction]; }
        
        int getX() const { return x; }
        int getY() const { return y; }
        
        void draw(sf::RenderWindow& window, int cellSize, bool isStart, bool isFinish) const {
            int px = x * cellSize;
            int py = y * cellSize;
            
            if(visited){
                sf::RectangleShape bg(sf::Vector2f(cellSize, cellSize));
                bg.setPosition(px, py);
                
                if(isStart) {
                    bg.setFillColor(sf::Color(50, 150, 50, 180));
                } else if(isFinish) {
                    bg.setFillColor(sf::Color(150, 50, 50, 180));
                } else {
                    bg.setFillColor(sf::Color(40, 40, 60));
                }
                window.draw(bg);
            }
            
            sf::Color wallColor(200, 200, 220);
            
            if(walls[0]) {
                sf::RectangleShape wall(sf::Vector2f(cellSize, 2));
                wall.setPosition(px, py);
                wall.setFillColor(wallColor);
                window.draw(wall);
            }
            if(walls[1]) {
                sf::RectangleShape wall(sf::Vector2f(2, cellSize));
                wall.setPosition(px + cellSize, py);
                wall.setFillColor(wallColor);
                window.draw(wall);
            }
            if(walls[2]) {
                sf::RectangleShape wall(sf::Vector2f(cellSize, 2));
                wall.setPosition(px, py + cellSize);
                wall.setFillColor(wallColor);
                window.draw(wall);
            }
            if(walls[3]) {
                sf::RectangleShape wall(sf::Vector2f(2, cellSize));
                wall.setPosition(px, py);
                wall.setFillColor(wallColor);
                window.draw(wall);
            }
        }
};

class Player : public MovableEntity {
    private:
        int cellSize;
        float visionRadius;
        sf::Clock animClock;
        
    public:
        Player(int startX, int startY, int cellSize, float visionRadius = 3.0f): MovableEntity(startX, startY), cellSize(cellSize), visionRadius(visionRadius) {}
        
        virtual ~Player() {}
        
        int getCellX() const { return static_cast<int>(x); }
        int getCellY() const { return static_cast<int>(y); }
        float getVisionRadius() const { return visionRadius; }
        
        void increaseVision(float amount) {
            visionRadius += amount;
            if(visionRadius > 15.0f) visionRadius = 15.0f;
        }
        
        
        virtual void move(int dx, int dy) override {
            MovableEntity::move(dx, dy);
        }
        
        bool isInVision(int cellX, int cellY) const {
            float dx = cellX - x;
            float dy = cellY - y;
            return (dx * dx + dy * dy) <= (visionRadius * visionRadius);
        }
        
       
        virtual void draw(sf::RenderWindow& window) const override {
            float px = x * cellSize + cellSize / 2;
            float py = y * cellSize + cellSize / 2;
        
            sf::CircleShape player(cellSize / 3, 3);
            player.setOrigin(cellSize / 3, cellSize / 3);
            player.setPosition(px, py);
            player.setRotation(direction * 90 + 90);
            player.setFillColor(sf::Color(255, 220, 100));
            window.draw(player);
            
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
        int startX, startY;
        int finishX, finishY;
        
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
            
            current->removeWall(dir);
            neighbors[idx]->removeWall((dir + 2) % 4);
            
            return neighbors[idx];
        }
        
    public:
        Maze(int cols, int rows, int cellSize) : cols(cols), rows(rows), cellSize(cellSize), generating(true),startX(0), startY(0), finishX(cols - 1), finishY(rows - 1) {
            
            grid.resize(rows);
            for(int y = 0; y < rows; y++) {
                grid[y].resize(cols);
                for(int x = 0; x < cols; x++) {
                    grid[y][x] = Cell(x, y);
                }
            }
            
            current = &grid[0][0];
            current->setVisited(true);
        }
        
        int getStartX() const { return startX; }
        int getStartY() const { return startY; }
        int getFinishX() const { return finishX; }
        int getFinishY() const { return finishY; }
        int getCols() const { return cols; }
        int getRows() const { return rows; }
        
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
        
        bool canMove(int x, int y, int dx, int dy) const {
            if(x + dx < 0 || x + dx >= cols || y + dy < 0 || y + dy >= rows)
                return false;
            
            const Cell& cell = grid[y][x];
            
            if(dx == 0 && dy == -1) return !cell.hasWall(0);
            if(dx == 1 && dy == 0) return !cell.hasWall(1);
            if(dx == 0 && dy == 1) return !cell.hasWall(2);
            if(dx == -1 && dy == 0) return !cell.hasWall(3);
            
            return false;
        }
        
        void draw(sf::RenderWindow& window) {
            for(int y = 0; y < rows; y++) {
                for(int x = 0; x < cols; x++) {
                    bool isStart = (x == startX && y == startY);
                    bool isFinish = (x == finishX && y == finishY);
                    grid[y][x].draw(window, cellSize, isStart, isFinish);
                }
            }
            
            if(generating && current) {
                sf::RectangleShape highlight(sf::Vector2f(cellSize - 4, cellSize - 4));
                highlight.setPosition(current->getX() * cellSize + 2, 
                                    current->getY() * cellSize + 2);
                highlight.setFillColor(sf::Color(100, 200, 255));
                window.draw(highlight);
            }
        }
        
        void drawWithVision(sf::RenderWindow& window, const Player& player) {
            for(int y = 0; y < rows; y++) {
                for(int x = 0; x < cols; x++) {
                    if(player.isInVision(x, y)) {
                        bool isStart = (x == startX && y == startY);
                        bool isFinish = (x == finishX && y == finishY);
                        grid[y][x].draw(window, cellSize, isStart, isFinish);
                    }
                }
            }
        }
};


class Game {
    private:
        static const int CELL_SIZE = 25;
        static const int COLS = 30;
        static const int ROWS = 20;
        static const int MAZE_WIDTH = 750;
        static const int MAZE_HEIGHT = 500;
        static const int WINDOW_WIDTH = 1000;
        static const int WINDOW_HEIGHT = 700;
        
        sf::RenderWindow window;
        GameState currentState;
        Maze* maze;
        Player* player;
        std::vector<Riddle*> riddles;
        std::vector<LeaderboardEntry> leaderboard;
        
        int currentRiddleIndex;
        std::string playerAnswer;
        sf::Clock timer;
        float elapsedTime;
        sf::Font gameFont;
        
        void createRiddles() {
            for(auto riddle : riddles) {
                delete riddle;
            }
            riddles.clear();

            struct RiddleData {
                std::string question;
                std::string answer;
                int difficulty; // 0=easy, 1=normal, 2=hard
            };

            std::vector<RiddleData> allRiddles = {
                {"I follow you silently and vanish in light, what am I?", "shadow", 0},
                {"The more of me you take, the darker your path becomes. What am I?", "darkness", 0},
                {"I can creep without legs, whisper without voice, and vanish when caught. What am I?", "wind", 1},
                {"I am always hungry, I must always be fed, the finger I touch will soon turn red. What am I?", "fire", 1},
                {"The more you have of me, the less you see. What am I?", "fog", 1},
                {"I am not alive, but I can grow; I don’t have lungs, but I need air; I don’t have a mouth, and I can drown. What am I?", "fire", 2},
                {"I appear in the night sky but vanish in the day, I can guide lost souls on their way. What am I?", "star", 0},
                {"I can be cracked, made, told, and played. What am I?", "joke", 0},
                {"I have a heart that doesn’t beat, a face without features, and a soul that roams. What am I?", "statue", 2},
                {"I never speak, but I reveal secrets in shadows. What am I?", "mirror", 1},
                {"You can’t see me, but I follow your every step; I only disappear in the dark. What am I?", "shadow", 0},
                {"I enter your home unseen, linger, and leave only when you call me by name. What am I?", "ghost", 2},
                {"I am light as a feather, yet the strongest man cannot hold me for long. What am I?", "breath", 1},
                {"I have one eye but cannot see, I am feared by sailors on stormy seas. What am I?", "needle", 2},
                {"I am always in front of you, but can never be seen. What am I?", "future", 2}
            };
            
            for(int i = 0; i < 4 && i < allRiddles.size(); i++){
                int posX = rand() % (COLS - 4) + 2;
                int posY = rand() % (ROWS - 4) + 2;
                
                Riddle* newRiddle;
                
                if(allRiddles[i].difficulty == 0){
                    newRiddle = new EasyRiddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE);
                } else if(allRiddles[i].difficulty == 2) {
                    newRiddle = new HardRiddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE);
                } else {
                    newRiddle = new Riddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE);
                }
                riddles.push_back(newRiddle);
            }
        }
        
        void loadScores() {
            leaderboard.clear();
            std::ifstream file("leaderboard.txt");
            
            if(file.is_open()) {
                std::string name;
                float time;
                while(file >> name >> time) {
                    leaderboard.push_back(LeaderboardEntry(name, time));
                }
                file.close();
            }
            
            std::sort(leaderboard.begin(), leaderboard.end());
        }
        
        void saveScores() {
            std::ofstream file("leaderboard.txt");
            
            if(file.is_open()) {
                for(const auto& entry : leaderboard) {
                    file << entry.name << " " << entry.time << "\n";
                }
                file.close();
            }
        }
        
        void addScore(const std::string& name, float time) {
            leaderboard.push_back(LeaderboardEntry(name, time));
            std::sort(leaderboard.begin(), leaderboard.end());
            
            if(leaderboard.size() > 10) {
                leaderboard.resize(10);
            }
            saveScores();
        }
        
        void checkForRiddle() {
            for(size_t i = 0; i < riddles.size(); i++) {
                bool riddleNotSolved = !riddles[i]->isSolved();
                bool playerOnRiddle = (player->getCellX() == riddles[i]->getX() && 
                                      player->getCellY() == riddles[i]->getY());
                
                if(riddleNotSolved && playerOnRiddle) {
                    currentState = RIDDLE_ACTIVE;
                    currentRiddleIndex = i;
                    playerAnswer.clear();
                    return;
                }
            }
        }
        
        void showWelcomeScreen() {
            window.clear(sf::Color(20, 20, 35));
            
            sf::Text title("THE ENLIGHTENED PATH", gameFont, 50);
            title.setPosition(WINDOW_WIDTH / 2 - 300, 150);
            title.setFillColor(sf::Color(200, 200, 220));
            title.setStyle(sf::Text::Bold);
            window.draw(title);
            
            sf::Text subtitle("Knowledge is your light...", gameFont, 25);
            subtitle.setPosition(WINDOW_WIDTH / 2 - 180, 220);
            subtitle.setFillColor(sf::Color(150, 150, 170));
            subtitle.setStyle(sf::Text::Italic);
            window.draw(subtitle);
            
            sf::Text instructions("Press SPACE to Start\nPress L for Leaderboard\nPress ESC to Exit", gameFont, 20);
            instructions.setPosition(WINDOW_WIDTH / 2 - 150, 350);
            instructions.setFillColor(sf::Color(180, 180, 200));
            window.draw(instructions);
        }
        
        void showLeaderboard() {
            window.clear(sf::Color(20, 20, 40));
            
            sf::Text title("LEADERBOARD", gameFont, 40);
            title.setPosition(WINDOW_WIDTH / 2 - 150, 50);
            title.setFillColor(sf::Color(255, 220, 100));
            window.draw(title);
            
            int yPos = 150;
            for(size_t i = 0; i < leaderboard.size() && i < 10; i++) {
                std::stringstream text;
                text << (i + 1) << ". " << leaderboard[i].name << " - " << (int)leaderboard[i].time << "s";
                
                sf::Text entry(text.str(), gameFont, 20);
                entry.setPosition(WINDOW_WIDTH / 2 - 150, yPos);
                entry.setFillColor(sf::Color(200, 200, 200));
                window.draw(entry);
                yPos += 40;
            }
            
            sf::Text back("Press ESC to go back", gameFont, 18);
            back.setPosition(WINDOW_WIDTH / 2 - 120, 600);
            back.setFillColor(sf::Color(150, 150, 150));
            window.draw(back);
        }
        
        void showMiniLeaderboard() {
            sf::RectangleShape box(sf::Vector2f(220, 300));
            box.setPosition(MAZE_WIDTH + 20, 20);
            box.setFillColor(sf::Color(30, 30, 50, 200));
            box.setOutlineColor(sf::Color(100, 100, 120));
            box.setOutlineThickness(2);
            window.draw(box);
            
            sf::Text title("Current Leaderboard", gameFont, 18);
            title.setPosition(MAZE_WIDTH + 40, 30);
            title.setFillColor(sf::Color(255, 220, 100));
            window.draw(title);
            
            int yPos = 70;
            for(size_t i = 0; i < leaderboard.size() && i < 5; i++) {
                std::stringstream text;
                text << (i + 1) << ". " << leaderboard[i].name.substr(0, 8) << " " << (int)leaderboard[i].time << "s";
                
                sf::Text entry(text.str(), gameFont, 14);
                entry.setPosition(MAZE_WIDTH + 35, yPos);
                entry.setFillColor(sf::Color(200, 200, 200));
                window.draw(entry);
                yPos += 35;
            }
        }
        
        void showRiddleBox() {
            sf::RectangleShape box(sf::Vector2f(MAZE_WIDTH - 40, 150));
            box.setPosition(20, MAZE_HEIGHT + 20);
            box.setFillColor(sf::Color(40, 40, 70, 230));
            box.setOutlineColor(sf::Color(255, 220, 100));
            box.setOutlineThickness(3);
            window.draw(box);

            if(currentRiddleIndex >= 0 && currentRiddleIndex < riddles.size()) {
                Riddle* activeRiddle = riddles[currentRiddleIndex];
                
                sf::Text question(activeRiddle->getQuestion(), gameFont, 16);
                question.setPosition(40, MAZE_HEIGHT + 35);
                question.setFillColor(sf::Color(255, 255, 255));
                window.draw(question);
                
                std::stringstream rewardText;
                rewardText << "Reward: +" << activeRiddle->getReward() << " vision";
                sf::Text reward(rewardText.str(), gameFont, 14);
                reward.setPosition(40, MAZE_HEIGHT + 60);
                reward.setFillColor(sf::Color(150, 255, 150));
                window.draw(reward);
                
                sf::RectangleShape inputBox(sf::Vector2f(MAZE_WIDTH - 80, 40));
                inputBox.setPosition(40, MAZE_HEIGHT + 90);
                inputBox.setFillColor(sf::Color(20, 20, 40));
                inputBox.setOutlineColor(sf::Color(100, 100, 150));
                inputBox.setOutlineThickness(2);
                window.draw(inputBox);
                
                sf::Text input("Answer: " + playerAnswer + "_", gameFont, 18);
                input.setPosition(50, MAZE_HEIGHT + 100);
                input.setFillColor(sf::Color(200, 200, 200));
                window.draw(input);
                
                sf::Text hint("Press ENTER to submit | ESC to close", gameFont, 12);
                hint.setPosition(40, MAZE_HEIGHT + 145);
                hint.setFillColor(sf::Color(150, 150, 150));
                window.draw(hint);
            }
        }
        
        void showGameInfo() {
            std::stringstream timeText;
            timeText << "Time: " << (int)elapsedTime << "s";
            sf::Text timeDisplay(timeText.str(), gameFont, 20);
            timeDisplay.setPosition(20, MAZE_HEIGHT + 180);
            timeDisplay.setFillColor(sf::Color(255, 220, 100));
            window.draw(timeDisplay);
            
            int solvedCount = 0;
            for(const auto& riddle : riddles) {
                if(riddle->isSolved()) {
                    solvedCount++;
                }
            }
            
            std::stringstream riddleText;
            riddleText << "Riddles: " << solvedCount << "/" << riddles.size();
            sf::Text riddleDisplay(riddleText.str(), gameFont, 20);
            riddleDisplay.setPosition(180, MAZE_HEIGHT + 180);
            riddleDisplay.setFillColor(sf::Color(200, 200, 255));
            window.draw(riddleDisplay);
            
            std::stringstream visionText;
            visionText << "Vision: " << (int)player->getVisionRadius();
            sf::Text visionDisplay(visionText.str(), gameFont, 20);
            visionDisplay.setPosition(380, MAZE_HEIGHT + 180);
            visionDisplay.setFillColor(sf::Color(150, 255, 150));
            window.draw(visionDisplay);
            
            sf::RectangleShape giveUpButton(sf::Vector2f(100, 35));
            giveUpButton.setPosition(MAZE_WIDTH - 120, MAZE_HEIGHT + 175);
            giveUpButton.setFillColor(sf::Color(150, 50, 50));
            window.draw(giveUpButton);
            
            sf::Text giveUpText("Give Up", gameFont, 16);
            giveUpText.setPosition(MAZE_WIDTH - 105, MAZE_HEIGHT + 183);
            giveUpText.setFillColor(sf::Color(255, 255, 255));
            window.draw(giveUpText);
        }
        
        void showRiddleMarkers() {
            for(const auto& riddle : riddles) {
                bool canSeeRiddle = player->isInVision(riddle->getX(), riddle->getY());
                
                if(!riddle->isSolved() && canSeeRiddle) {
                    riddle->draw(window);
                }
            }
        }
        
        void showVictoryScreen() {
            window.clear(sf::Color(20, 40, 20));
            
            sf::Text title("VICTORY!", gameFont, 60);
            title.setPosition(WINDOW_WIDTH / 2 - 150, 150);
            title.setFillColor(sf::Color(100, 255, 100));
            window.draw(title);
            
            std::stringstream timeText;
            timeText << "Time: " << (int)elapsedTime << " seconds";
            sf::Text timeDisplay(timeText.str(), gameFont, 30);
            timeDisplay.setPosition(WINDOW_WIDTH / 2 - 150, 250);
            timeDisplay.setFillColor(sf::Color(255, 255, 255));
            window.draw(timeDisplay);
            
            sf::Text instructions("Press SPACE to play again\nPress ESC to exit", gameFont, 20);
            instructions.setPosition(WINDOW_WIDTH / 2 - 150, 350);
            instructions.setFillColor(sf::Color(200, 200, 200));
            window.draw(instructions);
        }
        
        void showGameOverScreen() {
            window.clear(sf::Color(40, 20, 20));
            
            sf::Text title("GAME OVER", gameFont, 60);
            title.setPosition(WINDOW_WIDTH / 2 - 200, 150);
            title.setFillColor(sf::Color(255, 100, 100));
            window.draw(title);
            
            sf::Text instructions("Press SPACE to try again\nPress ESC to exit", gameFont, 20);
            instructions.setPosition(WINDOW_WIDTH / 2 - 150, 350);
            instructions.setFillColor(sf::Color(200, 200, 200));
            window.draw(instructions);
        }
        
    public:
        Game(): window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "The Enlightened Path"),currentState(WELCOME), maze(nullptr), player(nullptr), currentRiddleIndex(-1), elapsedTime(0){
            window.setFramerateLimit(60);
            if(!gameFont.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                cout << "Warning: Could not load font\n";
            }
            loadScores();
            srand(time(0));
        }
        
        ~Game() {
            delete maze;
            delete player;
            for(auto riddle : riddles) {
                delete riddle;
            }
        }
        
        void startNewGame() {
            delete maze;
            delete player;
            
            maze = new Maze(COLS, ROWS, CELL_SIZE);
            currentState = GENERATING;
            timer.restart();
            elapsedTime = 0;
        }
        
        void handleInput() {
            sf::Event event;
            while(window.pollEvent(event)) {
                if(event.type == sf::Event::Closed) {
                    window.close();
                }
                
                if(event.type == sf::Event::KeyPressed) {
                    if(currentState == WELCOME) {
                        if(event.key.code == sf::Keyboard::Space) {
                            startNewGame();
                        } 
                        else if(event.key.code == sf::Keyboard::L) {
                            currentState = LEADERBOARD_VIEW;
                        } 
                        else if(event.key.code == sf::Keyboard::Escape) {
                            window.close();
                        }
                    }
                    else if(currentState == LEADERBOARD_VIEW) {
                        if(event.key.code == sf::Keyboard::Escape) {
                            currentState = WELCOME;
                        }
                    }
                    else if(currentState == PLAYING) {
                        int moveX = 0;
                        int moveY = 0;
                        
                        if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
                            moveY = -1;
                        }
                        else if(event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
                            moveY = 1;
                        }
                        else if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) {
                            moveX = -1;
                        }
                        else if(event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) {
                            moveX = 1;
                        }
                        else if(event.key.code == sf::Keyboard::G) {
                            currentState = GAME_OVER;
                        }
                        bool canMove = maze->canMove(player->getCellX(), player->getCellY(), moveX, moveY);
                        if(canMove) {
                            player->move(moveX, moveY);
                            checkForRiddle();
                            bool reachedExit = (player->getCellX() == maze->getFinishX() &&  player->getCellY() == maze->getFinishY());
                            if(reachedExit) {
                                currentState = VICTORY;
                                addScore("Player", elapsedTime);
                            }
                        }
                    }else if(currentState == RIDDLE_ACTIVE) {
                        if(event.key.code == sf::Keyboard::Escape) {
                            currentState = PLAYING;
                        } else if(event.key.code == sf::Keyboard::Enter) {
                            std::string answer = playerAnswer;
                            std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
                            
                            bool validIndex = (currentRiddleIndex >= 0 && currentRiddleIndex < riddles.size());
                            bool correctAnswer = (answer == riddles[currentRiddleIndex]->getAnswer());
                            
                            if(validIndex && correctAnswer) {
                                riddles[currentRiddleIndex]->setSolved(true);
                                player->increaseVision(riddles[currentRiddleIndex]->getReward());
                                currentState = PLAYING;
                            }
                            playerAnswer.clear();
                        }else if(event.key.code == sf::Keyboard::Backspace && !playerAnswer.empty()) {
                            playerAnswer.pop_back();
                        }
                    }else if(currentState == VICTORY || currentState == GAME_OVER) {
                        if(event.key.code == sf::Keyboard::Space) {
                            startNewGame();
                        }else if(event.key.code == sf::Keyboard::Escape) {
                            currentState = WELCOME;
                        }
                    }
                }
                
                if(event.type == sf::Event::TextEntered && currentState == RIDDLE_ACTIVE) {
                    bool isPrintable = (event.text.unicode < 128 && event.text.unicode >= 32);
                    bool notTooLong = (playerAnswer.length() < 30);
                    
                    if(isPrintable && notTooLong) {
                        playerAnswer += static_cast<char>(event.text.unicode);
                    }
                }
            }
        }
        
        void updateGame() {
            if(currentState == GENERATING) {
                for(int i = 0; i < 5; i++) {
                    maze->step();
                }
                
                if(!maze->isGenerating()) {
                    player = new Player(maze->getStartX(), maze->getStartY(), CELL_SIZE, 3.0f);
                    createRiddles();
                    currentState = PLAYING;
                    timer.restart();
                }
            }
            
            if(currentState == PLAYING) {
                elapsedTime = timer.getElapsedTime().asSeconds();
            }
        }
        
        void draw() {
            window.clear(sf::Color(10, 10, 20));
            
            if(currentState == WELCOME) {
                showWelcomeScreen();
            }else if(currentState == LEADERBOARD_VIEW) {
                showLeaderboard();
            }else if(currentState == GENERATING) {
                maze->draw(window);
            }else if(currentState == PLAYING || currentState == RIDDLE_ACTIVE) {
                sf::RectangleShape darkness(sf::Vector2f(MAZE_WIDTH, MAZE_HEIGHT));
                darkness.setFillColor(sf::Color(0, 0, 0, 220));
                window.draw(darkness);
                
                maze->drawWithVision(window, *player);
                showRiddleMarkers();
                player->draw(window);
                
                float visionSize = player->getVisionRadius() * CELL_SIZE;
                sf::CircleShape visionCircle(visionSize);
                visionCircle.setOrigin(visionSize, visionSize);
                visionCircle.setPosition(player->getCellX() * CELL_SIZE + CELL_SIZE / 2,
                                        player->getCellY() * CELL_SIZE + CELL_SIZE / 2);
                visionCircle.setFillColor(sf::Color(255, 255, 255, 5));
                visionCircle.setOutlineColor(sf::Color(255, 255, 150, 30));
                visionCircle.setOutlineThickness(2);
                window.draw(visionCircle);
                
                showGameInfo();
                showMiniLeaderboard();
                
                if(currentState == RIDDLE_ACTIVE) {
                    showRiddleBox();
                }
            }else if(currentState == VICTORY) {
                showVictoryScreen();
            }else if(currentState == GAME_OVER) {
                showGameOverScreen();
            }
            
            window.display();
        }
        
        void run() {
            while(window.isOpen()) {
                handleInput();
                updateGame();
                draw();
            }
        }
};

int main() {
    Game game;
    game.run();
    return 0;
}