#ifndef GAME_H
#define GAME_H

#include "Maze.h"
#include "Player.h"
#include "Riddle.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum GameState { WELCOME, GENERATING, PLAYING, RIDDLE_ACTIVE, GAME_OVER, VICTORY, LEADERBOARD_VIEW };

struct LeaderboardEntry { std::string name; float time; LeaderboardEntry() : name(""), time(0) {} LeaderboardEntry(std::string n, float t) : name(n), time(t) {} bool operator<(const LeaderboardEntry& other) const { return time < other.time; } };

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

    void createRiddles();
    void loadScores();
    void saveScores();
    void addScore(const std::string& name, float time);
    void checkForRiddle();
    void showWelcomeScreen();
    void showLeaderboard();
    void showMiniLeaderboard();
    void showRiddleBox();
    void showGameInfo();
    void showRiddleMarkers();
    void showVictoryScreen();
    void showGameOverScreen();

public:
    Game();
    ~Game();
    void startNewGame();
    void handleInput();
    void updateGame();
    void draw();
    void run();
};

#endif // GAME_H
