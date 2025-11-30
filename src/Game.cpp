#include "Game.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iostream>

// Game.cpp: Implements the main application loop and UI.
// Responsibilities:
// - Manage game states (WELCOME, GENERATING, PLAYING, RIDDLE_ACTIVE, etc.)
// - Create and update Maze, Player and Riddle objects
// - Handle input and render frames using SFML
// The Game methods below are intentionally straightforward so a beginner
// can follow the flow from input -> update -> draw.

Game::Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "The Enlightened Path"), currentState(WELCOME), maze(nullptr), player(nullptr), currentRiddleIndex(-1), elapsedTime(0) {
    window.setFramerateLimit(60);
    // Try loading font from several common locations (project-local first)
    const char* fontCandidates[] = {
        "./fonts/arial.ttf",
        "./fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/arial.ttf",
        nullptr
    };

    bool fontLoaded = false;
    for(const char** p = fontCandidates; *p != nullptr; ++p) {
        if(gameFont.loadFromFile(*p)) {
            std::cout << "Loaded font: " << *p << "\n";
            fontLoaded = true;
            break;
        }
    }
    if(!fontLoaded) {
        std::cout << "Warning: Could not load any of the candidate fonts.\n";
        std::cout << "Put a TTF into ./fonts/ (e.g. fonts/DejaVuSans.ttf) or install fonts on the system.\n";
    }
    loadScores();
    srand((unsigned)time(0));
}

// createRiddles: populate the `riddles` vector with a small random set
// of riddles. Each riddle has a position and a difficulty (which affects reward).
// This is called when a new game starts so riddles are placed in the freshly
// generated maze.

Game::~Game() {
    delete maze;
    delete player;
    for(auto r : riddles) delete r;
}

void Game::createRiddles() {
    for(auto r : riddles) delete r;
    riddles.clear();
    struct RiddleData { std::string question; std::string answer; int difficulty; };
    std::vector<RiddleData> allRiddles = {
        {"I follow you silently and vanish in light, what am I?","shadow",0},
        {"The more of me you take, the darker your path becomes. What am I?","darkness",0},
        {"I can creep without legs, whisper without voice, and vanish when caught. What am I?","wind",1},
        {"I am always hungry, I must always be fed, the finger I touch will soon turn red. What am I?","fire",1},
        {"The more you have of me, the less you see. What am I?","fog",1},
        {"I am not alive, but I can grow; I don’t have lungs, but I need air; I don’t have a mouth, and I can drown. What am I?","fire",2},
        {"I appear in the night sky but vanish in the day, I can guide lost souls on their way. What am I?","star",0},
        {"I can be cracked, made, told, and played. What am I?","joke",0},
        {"I have a heart that doesn’t beat, a face without features, and a soul that roams. What am I?","statue",2},
        {"I never speak, but I reveal secrets in shadows. What am I?","mirror",1},
        {"You can’t see me, but I follow your every step; I only disappear in the dark. What am I?","shadow",0},
        {"I enter your home unseen, linger, and leave only when you call me by name. What am I?","ghost",2},
        {"I am light as a feather, yet the strongest man cannot hold me for long. What am I?","breath",1},
        {"I have one eye but cannot see, I am feared by sailors on stormy seas. What am I?","needle",2},
        {"I am always in front of you, but can never be seen. What am I?","future",2}
    };
    for(int i=0; i<4 && i < (int)allRiddles.size(); ++i) {
        int posX = rand() % (COLS - 4) + 2;
        int posY = rand() % (ROWS - 4) + 2;
        Riddle* newRiddle = nullptr;
        if(allRiddles[i].difficulty == 0) newRiddle = new EasyRiddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE);
        else if(allRiddles[i].difficulty == 2) newRiddle = new HardRiddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE);
        else newRiddle = new Riddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE);
        riddles.push_back(newRiddle);
    }
}

// loadScores / saveScores: simple persistence for the leaderboard.
// Scores are stored in `leaderboard.txt` where each line is `name time`.

void Game::loadScores() {
    leaderboard.clear();
    std::ifstream file("leaderboard.txt");
    if(file.is_open()) {
        std::string name; float timev;
        while(file >> name >> timev) leaderboard.push_back(LeaderboardEntry(name, timev));
        file.close();
    }
    std::sort(leaderboard.begin(), leaderboard.end());
}

void Game::saveScores() {
    std::ofstream file("leaderboard.txt");
    if(file.is_open()) {
        for(const auto& e : leaderboard) file << e.name << " " << e.time << "\n";
        file.close();
    }
}

void Game::addScore(const std::string& name, float time) {
    leaderboard.push_back(LeaderboardEntry(name, time));
    std::sort(leaderboard.begin(), leaderboard.end());
    if(leaderboard.size() > 10) leaderboard.resize(10);
    saveScores();
}

// checkForRiddle: called after player movement to detect whether the
// player has stepped onto a riddle cell. If so, switch to RIDDLE_ACTIVE
// to accept typed input for the answer.


void Game::checkForRiddle() {
    for(size_t i=0;i<riddles.size();++i) {
        bool riddleNotSolved = !riddles[i]->isSolved();
        bool playerOnRiddle = (player->getCellX() == (int)riddles[i]->getX() && player->getCellY() == (int)riddles[i]->getY());
        if(riddleNotSolved && playerOnRiddle) { currentState = RIDDLE_ACTIVE; currentRiddleIndex = (int)i; playerAnswer.clear(); return; }
    }
}

void Game::showWelcomeScreen() {
    window.clear(sf::Color(20,20,35));
    sf::Text title("THE ENLIGHTENED PATH", gameFont, 50);
    title.setPosition(WINDOW_WIDTH/2 - 300, 150);
    title.setFillColor(sf::Color(200,200,220)); title.setStyle(sf::Text::Bold);
    window.draw(title);
    sf::Text subtitle("Knowledge is your light...", gameFont, 25);
    subtitle.setPosition(WINDOW_WIDTH/2 - 180, 220); subtitle.setFillColor(sf::Color(150,150,170)); subtitle.setStyle(sf::Text::Italic);
    window.draw(subtitle);
    sf::Text instructions("Press SPACE to Start\nPress L for Leaderboard\nPress ESC to Exit", gameFont, 20);
    instructions.setPosition(WINDOW_WIDTH/2 - 150, 350); instructions.setFillColor(sf::Color(180,180,200)); window.draw(instructions);
}

void Game::showLeaderboard() {
    window.clear(sf::Color(20,20,40));
    sf::Text title("LEADERBOARD", gameFont, 40);
    title.setPosition(WINDOW_WIDTH/2 - 150, 50); title.setFillColor(sf::Color(255,220,100)); window.draw(title);
    int yPos = 150;
    for(size_t i=0;i<leaderboard.size() && i<10;i++) {
        std::stringstream text; text << (i+1) << ". " << leaderboard[i].name << " - " << (int)leaderboard[i].time << "s";
        sf::Text entry(text.str(), gameFont, 20); entry.setPosition(WINDOW_WIDTH/2 - 150, yPos); entry.setFillColor(sf::Color(200,200,200)); window.draw(entry);
        yPos += 40;
    }
    sf::Text back("Press ESC to go back", gameFont, 18); back.setPosition(WINDOW_WIDTH/2 - 120, 600); back.setFillColor(sf::Color(150,150,150)); window.draw(back);
}

void Game::showMiniLeaderboard() {
    sf::RectangleShape box(sf::Vector2f(220,300)); box.setPosition(MAZE_WIDTH + 20, 20); box.setFillColor(sf::Color(30,30,50,200)); box.setOutlineColor(sf::Color(100,100,120)); box.setOutlineThickness(2); window.draw(box);
    sf::Text title("Current Leaderboard", gameFont, 18); title.setPosition(MAZE_WIDTH + 40, 30); title.setFillColor(sf::Color(255,220,100)); window.draw(title);
    int yPos = 70;
    for(size_t i=0;i<leaderboard.size() && i<5;i++) {
        std::stringstream text; text << (i+1) << ". " << leaderboard[i].name.substr(0,8) << " " << (int)leaderboard[i].time << "s";
        sf::Text entry(text.str(), gameFont, 14); entry.setPosition(MAZE_WIDTH + 35, yPos); entry.setFillColor(sf::Color(200,200,200)); window.draw(entry); yPos += 35;
    }
}

void Game::showRiddleBox() {
    sf::RectangleShape box(sf::Vector2f(MAZE_WIDTH - 40, 150)); box.setPosition(20, MAZE_HEIGHT + 20); box.setFillColor(sf::Color(40,40,70,230)); box.setOutlineColor(sf::Color(255,220,100)); box.setOutlineThickness(3); window.draw(box);
    if(currentRiddleIndex >= 0 && currentRiddleIndex < (int)riddles.size()) {
        Riddle* activeRiddle = riddles[currentRiddleIndex];
        sf::Text question(activeRiddle->getQuestion(), gameFont, 16); question.setPosition(40, MAZE_HEIGHT + 35); question.setFillColor(sf::Color(255,255,255)); window.draw(question);
        std::stringstream rewardText; rewardText << "Reward: +" << activeRiddle->getReward() << " vision";
        sf::Text reward(rewardText.str(), gameFont, 14); reward.setPosition(40, MAZE_HEIGHT + 60); reward.setFillColor(sf::Color(150,255,150)); window.draw(reward);
        sf::RectangleShape inputBox(sf::Vector2f(MAZE_WIDTH - 80, 40)); inputBox.setPosition(40, MAZE_HEIGHT + 90); inputBox.setFillColor(sf::Color(20,20,40)); inputBox.setOutlineColor(sf::Color(100,100,150)); inputBox.setOutlineThickness(2); window.draw(inputBox);
        sf::Text input("Answer: " + playerAnswer + "_", gameFont, 18); input.setPosition(50, MAZE_HEIGHT + 100); input.setFillColor(sf::Color(200,200,200)); window.draw(input);
        sf::Text hint("Press ENTER to submit | ESC to close", gameFont, 12); hint.setPosition(40, MAZE_HEIGHT + 145); hint.setFillColor(sf::Color(150,150,150)); window.draw(hint);
    }
}

void Game::showGameInfo() {
    std::stringstream timeText; timeText << "Time: " << (int)elapsedTime << "s"; sf::Text timeDisplay(timeText.str(), gameFont, 20); timeDisplay.setPosition(20, MAZE_HEIGHT + 180); timeDisplay.setFillColor(sf::Color(255,220,100)); window.draw(timeDisplay);
    int solvedCount = 0; for(const auto& r : riddles) if(r->isSolved()) ++solvedCount;
    std::stringstream riddleText; riddleText << "Riddles: " << solvedCount << "/" << riddles.size(); sf::Text riddleDisplay(riddleText.str(), gameFont, 20); riddleDisplay.setPosition(180, MAZE_HEIGHT + 180); riddleDisplay.setFillColor(sf::Color(200,200,255)); window.draw(riddleDisplay);
    std::stringstream visionText; visionText << "Vision: " << (int)player->getVisionRadius(); sf::Text visionDisplay(visionText.str(), gameFont, 20); visionDisplay.setPosition(380, MAZE_HEIGHT + 180); visionDisplay.setFillColor(sf::Color(150,255,150)); window.draw(visionDisplay);
    sf::RectangleShape giveUpButton(sf::Vector2f(100,35)); giveUpButton.setPosition(MAZE_WIDTH - 120, MAZE_HEIGHT + 175); giveUpButton.setFillColor(sf::Color(150,50,50)); window.draw(giveUpButton);
    sf::Text giveUpText("Give Up", gameFont, 16); giveUpText.setPosition(MAZE_WIDTH - 105, MAZE_HEIGHT + 183); giveUpText.setFillColor(sf::Color(255,255,255)); window.draw(giveUpText);
}

void Game::showRiddleMarkers() {
    for(const auto& r : riddles) {
        bool canSeeRiddle = player->isInVision(r->getX(), r->getY());
        if(!r->isSolved() && canSeeRiddle) r->draw(window);
    }
}

void Game::showVictoryScreen() {
    window.clear(sf::Color(20,40,20));
    sf::Text title("VICTORY!", gameFont, 60); title.setPosition(WINDOW_WIDTH/2 - 150, 150); title.setFillColor(sf::Color(100,255,100)); window.draw(title);
    std::stringstream timeText; timeText << "Time: " << (int)elapsedTime << " seconds"; sf::Text timeDisplay(timeText.str(), gameFont, 30); timeDisplay.setPosition(WINDOW_WIDTH/2 - 150, 250); timeDisplay.setFillColor(sf::Color(255,255,255)); window.draw(timeDisplay);
    sf::Text instructions("Press SPACE to play again\nPress ESC to exit", gameFont, 20); instructions.setPosition(WINDOW_WIDTH/2 - 150, 350); instructions.setFillColor(sf::Color(200,200,200)); window.draw(instructions);
}

void Game::showGameOverScreen() {
    window.clear(sf::Color(40,20,20));
    sf::Text title("GAME OVER", gameFont, 60); title.setPosition(WINDOW_WIDTH/2 - 200, 150); title.setFillColor(sf::Color(255,100,100)); window.draw(title);
    sf::Text instructions("Press SPACE to try again\nPress ESC to exit", gameFont, 20); instructions.setPosition(WINDOW_WIDTH/2 - 150, 350); instructions.setFillColor(sf::Color(200,200,200)); window.draw(instructions);
}

void Game::startNewGame() {
    delete maze; delete player; maze = new Maze(COLS, ROWS, CELL_SIZE); currentState = GENERATING; timer.restart(); elapsedTime = 0;
}

// handleInput: processes SFML events (keyboard and text input). Key areas:
// - State transitions (space, esc, L)
// - Player movement when playing
// - Text input handling when a riddle box is active

void Game::handleInput() {
    sf::Event event;
    while(window.pollEvent(event)) {
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed) {
            if(currentState == WELCOME) {
                if(event.key.code == sf::Keyboard::Space) startNewGame();
                else if(event.key.code == sf::Keyboard::L) currentState = LEADERBOARD_VIEW;
                else if(event.key.code == sf::Keyboard::Escape) window.close();
            } else if(currentState == LEADERBOARD_VIEW) {
                if(event.key.code == sf::Keyboard::Escape) currentState = WELCOME;
            } else if(currentState == PLAYING) {
                int moveX = 0, moveY = 0;
                if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) moveY = -1;
                else if(event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) moveY = 1;
                else if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) moveX = -1;
                else if(event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) moveX = 1;
                else if(event.key.code == sf::Keyboard::G) currentState = GAME_OVER;
                bool canMove = maze->canMove(player->getCellX(), player->getCellY(), moveX, moveY);
                if(canMove) { player->move(moveX, moveY); checkForRiddle(); bool reachedExit = (player->getCellX() == maze->getFinishX() && player->getCellY() == maze->getFinishY()); if(reachedExit) { currentState = VICTORY; addScore("Player", elapsedTime); } }
            } else if(currentState == RIDDLE_ACTIVE) {
                if(event.key.code == sf::Keyboard::Escape) currentState = PLAYING;
                else if(event.key.code == sf::Keyboard::Enter) {
                    std::string answer = playerAnswer; std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
                    bool validIndex = (currentRiddleIndex >= 0 && currentRiddleIndex < (int)riddles.size());
                    bool correctAnswer = validIndex && (answer == riddles[currentRiddleIndex]->getAnswer());
                    if(validIndex && correctAnswer) { riddles[currentRiddleIndex]->setSolved(true); player->increaseVision(riddles[currentRiddleIndex]->getReward()); currentState = PLAYING; }
                    playerAnswer.clear();
                } else if(event.key.code == sf::Keyboard::Backspace && !playerAnswer.empty()) playerAnswer.pop_back();
            } else if(currentState == VICTORY || currentState == GAME_OVER) {
                if(event.key.code == sf::Keyboard::Space) startNewGame();
                else if(event.key.code == sf::Keyboard::Escape) currentState = WELCOME;
            }
        }
        if(event.type == sf::Event::TextEntered && currentState == RIDDLE_ACTIVE) {
            bool isPrintable = (event.text.unicode < 128 && event.text.unicode >= 32);
            bool notTooLong = (playerAnswer.length() < 30);
            if(isPrintable && notTooLong) playerAnswer += static_cast<char>(event.text.unicode);
        }
    }
}

// updateGame: advance maze generation while in GENERATING state and
// update timers while PLAYING.

void Game::updateGame() {
    if(currentState == GENERATING) {
        for(int i=0;i<5;i++) maze->step();
        if(!maze->isGenerating()) { player = new Player(maze->getStartX(), maze->getStartY(), CELL_SIZE, 3.0f); createRiddles(); currentState = PLAYING; timer.restart(); }
    }
    if(currentState == PLAYING) elapsedTime = timer.getElapsedTime().asSeconds();
}

// draw: render the correct screen depending on the current state. While
// playing the scene composes a darkness overlay and only renders cells
// the player can see via `Maze::drawWithVision`.

void Game::draw() {
    window.clear(sf::Color(10,10,20));
    if(currentState == WELCOME) showWelcomeScreen();
    else if(currentState == LEADERBOARD_VIEW) showLeaderboard();
    else if(currentState == GENERATING) maze->draw(window);
    else if(currentState == PLAYING || currentState == RIDDLE_ACTIVE) {
        sf::RectangleShape darkness(sf::Vector2f(MAZE_WIDTH, MAZE_HEIGHT)); darkness.setFillColor(sf::Color(0,0,0,220)); window.draw(darkness);
        maze->drawWithVision(window, *player);
        showRiddleMarkers();
        player->draw(window);
        float visionSize = player->getVisionRadius() * CELL_SIZE;
        sf::CircleShape visionCircle(visionSize); visionCircle.setOrigin(visionSize, visionSize);
        visionCircle.setPosition(player->getCellX() * CELL_SIZE + CELL_SIZE / 2, player->getCellY() * CELL_SIZE + CELL_SIZE / 2);
        visionCircle.setFillColor(sf::Color(255,255,255,5)); visionCircle.setOutlineColor(sf::Color(255,255,150,30)); visionCircle.setOutlineThickness(2); window.draw(visionCircle);
        showGameInfo(); showMiniLeaderboard();
        if(currentState == RIDDLE_ACTIVE) showRiddleBox();
    } else if(currentState == VICTORY) showVictoryScreen();
    else if(currentState == GAME_OVER) showGameOverScreen();
    window.display();
}

// run: main loop that repeatedly handles input, updates game logic and draws
// until the window is closed.


void Game::run() {
    while(window.isOpen()) { handleInput(); updateGame(); draw(); }
}
