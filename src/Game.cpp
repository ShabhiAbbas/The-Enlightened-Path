#include "Game.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <random>
#include <unordered_map>



Game::Game() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "The Enlightened Path"), currentState(WELCOME), maze(nullptr), player(nullptr), currentRiddleIndex(-1), elapsedTime(0), playerDeadThisFrame(false), previousState(WELCOME) {
    window.setFramerateLimit(60);

 
    if(welcomeTexture.loadFromFile("Images/welcome.jpg")) {
        welcomeSprite.setTexture(welcomeTexture);
        sf::Vector2u size = welcomeTexture.getSize();
        welcomeSprite.setScale(1000.0f / size.x, 700.0f / size.y);
    } else {
        std::cout << "Warning: welcome.jpg not found (check Images folder).\n";
    }

 
    if(gameOverTexture.loadFromFile("Images/gameover.jpg")) {
        gameOverSprite.setTexture(gameOverTexture);
        sf::Vector2u size = gameOverTexture.getSize();
        gameOverSprite.setScale(1000.0f / size.x, 700.0f / size.y);
    } else {
        std::cout << "Warning: gameover.jpg not found (check Images folder).\n";
    }

 
    if (!bgMusic.openFromFile("haunted.wav")) {
        std::cout << "Error: Could not load haunted.wav" << std::endl;
    } else {
        bgMusic.setLoop(true); 
        bgMusic.setVolume(50);  
        bgMusic.play(); 
    }
  
    playerName = "";

 
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
    }

    loadScores();
    srand((unsigned)time(0));
}

Game::~Game() {
    delete maze;
    delete player;
    for(auto r : riddles) delete r;
    for(auto e : enemies) delete e;
}

void Game::createRiddles() {
    for(auto r : riddles) delete r;
    riddles.clear();

    struct RiddleData { 
        std::string question; 
        std::string answer; 
        int difficulty; 
        RiddleRewardType rewardType;
    };

    std::vector<RiddleData> allRiddles;

    std::ifstream rf("src/riddles.txt");
    if(!rf.is_open()) {
        std::ofstream wf("riddles.txt");
        if(wf.is_open()) {
            wf << "I follow you silently and vanish in light, what am I?|shadow|0|VISION_REWARD\n";
            wf << "The more of me you take, the darker your path becomes. What am I?|darkness|0|INVISIBILITY_REWARD\n";
            wf << "I can creep without legs, whisper without voice, and vanish when caught. What am I?|wind|1|VISION_REWARD\n";
            wf << "I am always hungry, I must always be fed, the finger I touch will soon turn red. What am I?|fire|1|KILL_POWER_REWARD\n";
            wf << "The more you have of me, the less you see. What am I?|fog|1|INVISIBILITY_REWARD\n";
            wf << "I am not alive, but I can grow; I don't have lungs, but I need air; I don't have a mouth, and I can drown. What am I?|fire|2|KILL_POWER_REWARD\n";
            wf << "I appear in the night sky but vanish in the day, I can guide lost souls on their way. What am I?|star|0|VISION_REWARD\n";
            wf << "I can be cracked, made, told, and played. What am I?|joke|0|INVISIBILITY_REWARD\n";
            wf << "I have a heart that doesn't beat, a face without features, and a soul that roams. What am I?|statue|2|KILL_POWER_REWARD\n";
            wf << "I never speak, but I reveal secrets in shadows. What am I?|mirror|1|INVISIBILITY_REWARD\n";
            wf << "You can't see me, but I follow your every step; I only disappear in the dark. What am I?|shadow|0|VISION_REWARD\n";
            wf << "I enter your home unseen, linger, and leave only when you call me by name. What am I?|ghost|2|KILL_POWER_REWARD\n";
            wf << "I am light as a feather, yet the strongest man cannot hold me for long. What am I?|breath|1|INVISIBILITY_REWARD\n";
            wf << "I have one eye but cannot see, I am feared by sailors on stormy seas. What am I?|needle|2|KILL_POWER_REWARD\n";
            wf << "I am always in front of you, but can never be seen. What am I?|future|2|VISION_REWARD\n";
            wf.close();
        }
        rf.open("riddles.txt");
    }

    std::string line;
    while(rf.is_open() && std::getline(rf, line)) {
        if(line.empty()) continue;
        if(line.size() > 0 && line[0] == '#') continue; 
        std::vector<std::string> parts;
        size_t pos = 0;
        while(true) {
            size_t p = line.find('|', pos);
            if(p == std::string::npos) { parts.push_back(line.substr(pos)); break; }
            parts.push_back(line.substr(pos, p - pos));
            pos = p + 1;
        }
        if(parts.size() < 4) continue;
        RiddleData d;
        d.question = parts[0];
        d.answer = parts[1];
        try { d.difficulty = std::stoi(parts[2]); } catch(...) { d.difficulty = 1; }
        std::string rt = parts[3];
        if(rt == "VISION_REWARD") d.rewardType = VISION_REWARD;
        else if(rt == "INVISIBILITY_REWARD") d.rewardType = INVISIBILITY_REWARD;
        else if(rt == "KILL_POWER_REWARD") d.rewardType = KILL_POWER_REWARD;
        else if(rt == "HEALTH_REWARD") d.rewardType = HEALTH_REWARD;
        else d.rewardType = VISION_REWARD;
        allRiddles.push_back(d);
    }
    if(rf.is_open()) rf.close();

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(allRiddles.begin(), allRiddles.end(), rng);
    int choose = std::min(10, (int)allRiddles.size());
    for(int i = 0; i < choose; ++i) {
        int posX = rand() % (COLS - 4) + 2;
        int posY = rand() % (ROWS - 4) + 2;
        Riddle* newRiddle = nullptr;
        if(allRiddles[i].difficulty == 0)
            newRiddle = new EasyRiddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE, allRiddles[i].rewardType);
        else if(allRiddles[i].difficulty == 2)
            newRiddle = new HardRiddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE, allRiddles[i].rewardType);
        else
            newRiddle = new Riddle(allRiddles[i].question, allRiddles[i].answer, posX, posY, CELL_SIZE, allRiddles[i].rewardType);
        riddles.push_back(newRiddle);
    }
}

void Game::spawnEnemies() {
    for(auto e : enemies) delete e;
    enemies.clear();
    
    int numEnemies = 3 + rand() % 3;
    const float enemyDetection = 5.0f;
    const int minDistance = static_cast<int>(enemyDetection) + 2; 
    int startX = player->getCellX();
    int startY = player->getCellY();
    for(int i = 0; i < numEnemies; ++i) {
        int ex, ey;
        int attempts = 0;
        do {
            ex = rand() % COLS;
            ey = rand() % ROWS;
            ++attempts;
        } while(((abs(ex - startX) < minDistance) && (abs(ey - startY) < minDistance)) || (ex == maze->getFinishX() && ey == maze->getFinishY()) && attempts < 200);
        
        if((abs(ex - startX) < minDistance) && (abs(ey - startY) < minDistance)) {
            ex = std::min(COLS-1, startX + minDistance);
            ey = std::min(ROWS-1, startY + minDistance);
        }
        
        Enemy* newEnemy = new Enemy(ex, ey, CELL_SIZE, enemyDetection);
        enemies.push_back(newEnemy);
    }
}

void Game::updateEnemies() {
    for(auto e : enemies) {
        if(!e->dead()) {
            if(e->isInDetectionRange(*player) && !player->getIsInvisible()) {
                e->moveTowardPlayer(*player, COLS, ROWS, maze);
            }
        }
    }
}

void Game::checkEnemyCollisions() {
    for(auto e : enemies) {
        if(!e->dead()) {
            int px = player->getCellX();
            int py = player->getCellY();
            int ex = static_cast<int>(e->getX());
            int ey = static_cast<int>(e->getY());
            
            if(px == ex && py == ey) {
                if(player->getCanKillEnemies()) {
                    e->takeDamage(10.0f);
                } else if(!player->getIsInvisible()) {
                    player->takeDamage(1.0f);
                
                    e->takeDamage(10.0f);
                    int newX, newY;
                    int attempts = 0;
                    do {
                        newX = rand() % COLS;
                        newY = rand() % ROWS;
                        ++attempts;
                    } while((newX == maze->getFinishX() && newY == maze->getFinishY()) && attempts < 100);
                    e->setPosition(static_cast<float>(newX), static_cast<float>(newY));
                }
            }
        }
    }
}

void Game::checkBulletCollisions() {
    auto& bullets = player->getBullets();
    for(size_t i = 0; i < bullets.size(); ++i) {
        if(!bullets[i].active) continue;
        
        int bx = static_cast<int>(bullets[i].x);
        int by = static_cast<int>(bullets[i].y);
        
        for(auto e : enemies) {
            if(!e->dead()) {
                int ex = static_cast<int>(e->getX());
                int ey = static_cast<int>(e->getY());
                
                if(bx == ex && by == ey) {
                    e->takeDamage(10.0f);
                    bullets[i].active = false;
                    break;
                }
            }
        }
    }
}

void Game::loadScores() {
    leaderboard.clear();

    std::ifstream file("leaderboard.txt");
    if(file.is_open()) {
        std::string name;
        float timev;
        std::unordered_map<std::string, float> bestTimes;
        while(file >> name >> timev) {
            auto it = bestTimes.find(name);
            if(it == bestTimes.end() || timev < it->second) bestTimes[name] = timev;
        }
        file.close();
        for(const auto &p : bestTimes) leaderboard.push_back(LeaderboardEntry(p.first, p.second));
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

    bool updated = false;
    for(auto &e : leaderboard) {
        if(e.name == name) {
            if(time < e.time) {
                e.time = time;
            }
            updated = true;
            break;
        }
    }
    if(!updated) {
        leaderboard.push_back(LeaderboardEntry(name, time));
    }
    std::sort(leaderboard.begin(), leaderboard.end());
    if(leaderboard.size() > 10) leaderboard.resize(10);
    saveScores();
}

void Game::checkForRiddle() {
    for(size_t i = 0; i < riddles.size(); ++i) {
        bool riddleNotSolved = !riddles[i]->isSolved();
        bool playerOnRiddle = (player->getCellX() == (int)riddles[i]->getX() && player->getCellY() == (int)riddles[i]->getY());
        if(riddleNotSolved && playerOnRiddle) { 
            currentState = RIDDLE_ACTIVE; 
            currentRiddleIndex = (int)i; 
            playerAnswer.clear(); 
            return; 
        }
    }
}

void Game::showWelcomeScreen() {
    window.draw(welcomeSprite);

    sf::RectangleShape overlay(sf::Vector2f(600, 400));
    overlay.setPosition(WINDOW_WIDTH/2 - 300, 150);
    overlay.setFillColor(sf::Color(0, 0, 0, 150)); 
    window.draw(overlay);

    sf::Text title("THE ENLIGHTENED PATH", gameFont, 50);
    title.setPosition(WINDOW_WIDTH / 2 - 300, 170);
    title.setFillColor(sf::Color(200, 200, 220)); 
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    sf::Text namePrompt("Enter your name:", gameFont, 25);
    namePrompt.setPosition(WINDOW_WIDTH / 2 - 100, 280);
    namePrompt.setFillColor(sf::Color(255, 255, 255));
    window.draw(namePrompt);

    sf::RectangleShape nameBox(sf::Vector2f(300, 40));
    nameBox.setPosition(WINDOW_WIDTH / 2 - 150, 320);
    nameBox.setFillColor(sf::Color(50, 50, 50));
    nameBox.setOutlineColor(sf::Color::White);
    nameBox.setOutlineThickness(2);
    window.draw(nameBox);

    sf::Text nameDisplay(playerName + "_", gameFont, 25);
    nameDisplay.setPosition(WINDOW_WIDTH / 2 - 140, 325);
    nameDisplay.setFillColor(sf::Color::Yellow);
    window.draw(nameDisplay);

    sf::Text instructions("Press ENTER to Start\nPress TAB for Leaderboard\nPress ESC to Exit", gameFont, 20);
    instructions.setPosition(WINDOW_WIDTH / 2 - 120, 450); 
    instructions.setFillColor(sf::Color(180, 180, 200)); 
    window.draw(instructions);
}

void Game::showLeaderboard() {
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

void Game::showMiniLeaderboard() {
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

void Game::showRiddleBox() {
    sf::RectangleShape box(sf::Vector2f(MAZE_WIDTH - 40, 150)); 
    box.setPosition(20, MAZE_HEIGHT + 20); 
    box.setFillColor(sf::Color(40, 40, 70, 230)); 
    box.setOutlineColor(sf::Color(255, 220, 100)); 
    box.setOutlineThickness(3); 
    window.draw(box);
    
    if(currentRiddleIndex >= 0 && currentRiddleIndex < (int)riddles.size()) {
        Riddle* activeRiddle = riddles[currentRiddleIndex];
        
        sf::Text question(activeRiddle->getQuestion(), gameFont, 16); 
        question.setPosition(40, MAZE_HEIGHT + 35); 
        question.setFillColor(sf::Color(255, 255, 255)); 
        window.draw(question);
        
        std::stringstream rewardText;
        RiddleRewardType rt = activeRiddle->getRewardType();

        if (rt == VISION_REWARD) {
            rewardText << "Reward: +" << activeRiddle->getReward()
                       << " vision";

        } else if (rt == INVISIBILITY_REWARD) {
            rewardText << "Reward: Invisibility";

        } else if (rt == KILL_POWER_REWARD) {
            rewardText << "Reward: Kill Power + " 
                       << GameConstants::KILL_POWER_AMMO_REWARD 
                       << " Ammo";

        } else if (rt == HEALTH_REWARD) {
            rewardText << "Reward: +" << activeRiddle->getReward()
                       << " health";
        }
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

void Game::showGameInfo() {
    std::stringstream timeText; 
    timeText << "Time: " << (int)elapsedTime << "s"; 
    sf::Text timeDisplay(timeText.str(), gameFont, 20); 
    timeDisplay.setPosition(20, MAZE_HEIGHT + 180); 
    timeDisplay.setFillColor(sf::Color(255, 220, 100)); 
    window.draw(timeDisplay);
    
    int solvedCount = 0; 
    for(const auto& r : riddles) if(r->isSolved()) ++solvedCount;
    
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
    
    std::stringstream ammoText; 
    ammoText << "Ammo: " << player->getAmmo(); 
    sf::Text ammoDisplay(ammoText.str(), gameFont, 20); 
    ammoDisplay.setPosition(550, MAZE_HEIGHT + 180); 
    ammoDisplay.setFillColor(sf::Color(255, 200, 100)); 
    window.draw(ammoDisplay);
}

void Game::showHealthBar() {
    int barX = 20;
    int barY = MAZE_HEIGHT + 20;
    
    sf::RectangleShape healthBarBg(sf::Vector2f(250, 30));
    healthBarBg.setPosition(barX, barY);
    healthBarBg.setFillColor(sf::Color(50, 50, 50));
    healthBarBg.setOutlineColor(sf::Color(100, 100, 100));
    healthBarBg.setOutlineThickness(2);
    window.draw(healthBarBg);
    
    float healthPercent = player->getHealth() / player->getMaxHealth();
    sf::RectangleShape healthBarFill(sf::Vector2f(250 * healthPercent, 30));
    healthBarFill.setPosition(barX, barY);
    
    if(player->getHealth() > 2.0f) {
        healthBarFill.setFillColor(sf::Color(0, 255, 0)); 
    } else if(player->getHealth() > 1.0f) {
        healthBarFill.setFillColor(sf::Color(255, 255, 0)); 
    } else {
        healthBarFill.setFillColor(sf::Color(255, 0, 0)); 
    }
    
    window.draw(healthBarFill);
    
    std::stringstream healthText;
    healthText << "HP: " << (int)player->getHealth() << "/" << (int)player->getMaxHealth();
    sf::Text healthLabel(healthText.str(), gameFont, 14);
    healthLabel.setPosition(barX + 10, barY + 6);
    healthLabel.setFillColor(sf::Color(255, 255, 255));
    window.draw(healthLabel);
}

void Game::showRiddleMarkers() {
    for(const auto& r : riddles) {
        bool canSeeRiddle = player->isInVision(r->getX(), r->getY());
        if(!r->isSolved() && canSeeRiddle) r->draw(window);
    }
}

void Game::showVictoryScreen() {
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

void Game::showGameOverScreen() {
    window.draw(gameOverSprite);

    sf::Text instructions("Press SPACE to try again\nPress ESC to exit", gameFont, 20); 
    instructions.setPosition(WINDOW_WIDTH / 2 - 150, 350); 
    instructions.setFillColor(sf::Color(200, 200, 200)); 
    window.draw(instructions);
}

void Game::startNewGame() {
    delete maze; 
    delete player; 
    maze = new Maze(COLS, ROWS, CELL_SIZE); 
    currentState = GENERATING; 
    timer.restart(); 
    elapsedTime = 0;
}

void Game::handleInput() {
    sf::Event event;
    while(window.pollEvent(event)) {
        if(event.type == sf::Event::Closed) window.close();
        if(event.type == sf::Event::KeyPressed) {
      
            if(currentState == WELCOME) {
                if(event.key.code == sf::Keyboard::Enter && !playerName.empty()) {
                    startNewGame();
                }
                else if(event.key.code == sf::Keyboard::Tab) currentState = LEADERBOARD_VIEW;
                else if(event.key.code == sf::Keyboard::Escape) window.close();
         
            } else if(currentState == LEADERBOARD_VIEW) {
                if(event.key.code == sf::Keyboard::Escape) currentState = WELCOME;
            
         
            } else if(currentState == PLAYING) {
                int moveX = 0, moveY = 0;
                if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) moveY = -1;
                else if(event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) moveY = 1;
                else if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::Left) moveX = -1;
                else if(event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right) moveX = 1;
                else if(event.key.code == sf::Keyboard::Space) player->fire();
                else if(event.key.code == sf::Keyboard::G) currentState = GAME_OVER;
                
                if(moveX != 0 || moveY != 0) {
                    bool canMove = maze->canMove(player->getCellX(), player->getCellY(), moveX, moveY);
                    if(canMove) { 
                        player->move(moveX, moveY); 
                        checkForRiddle(); 
                        bool reachedExit = (player->getCellX() == maze->getFinishX() && player->getCellY() == maze->getFinishY()); 
                        
                    
                        if(reachedExit) { 
                            currentState = VICTORY; 
                            addScore(playerName, elapsedTime); 
                        }
                    }
                }
            
            } else if(currentState == RIDDLE_ACTIVE) {
                if(event.key.code == sf::Keyboard::Escape) currentState = PLAYING;
                else if(event.key.code == sf::Keyboard::Enter) {
                    std::string answer = playerAnswer; 
                    std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);
                    bool validIndex = (currentRiddleIndex >= 0 && currentRiddleIndex < (int)riddles.size());
                    bool correctAnswer = validIndex && (answer == riddles[currentRiddleIndex]->getAnswer());
                    if(validIndex && correctAnswer) { 
                        riddles[currentRiddleIndex]->setSolved(true);
                        
                        RiddleRewardType rewardType =riddles[currentRiddleIndex]->getRewardType();

                        if (rewardType == VISION_REWARD) {
                            player->increaseVision(
                                riddles[currentRiddleIndex]->getReward());

                        } else if (rewardType == INVISIBILITY_REWARD) {
                        
                            player->setInvisible(true);

                        } else if (rewardType == KILL_POWER_REWARD) {
                         
                            player->setCanKillEnemies(true);
                            player->addAmmo(
                                GameConstants::KILL_POWER_AMMO_REWARD);

                        } else if (rewardType == HEALTH_REWARD) {
                            player->increaseHealth(
                                riddles[currentRiddleIndex]->getReward());
                        }
                        
                        currentState = PLAYING; 
                    }
                    playerAnswer.clear();
                } else if(event.key.code == sf::Keyboard::Backspace && !playerAnswer.empty()) playerAnswer.pop_back();
        
            } else if(currentState == VICTORY || currentState == GAME_OVER) {
                if(event.key.code == sf::Keyboard::Space) startNewGame();
                else if(event.key.code == sf::Keyboard::Escape) currentState = WELCOME;
            }
        }
    
        if(event.type == sf::Event::TextEntered) {
          
            if (currentState == RIDDLE_ACTIVE) {
                bool isPrintable = (event.text.unicode < 128 && event.text.unicode >= 32);
                bool notTooLong = (playerAnswer.length() < 30);
                if(isPrintable && notTooLong) playerAnswer += static_cast<char>(event.text.unicode);
                else if (event.text.unicode == 8 && !playerAnswer.empty()) playerAnswer.pop_back(); // Backspace
            }
            else if (currentState == WELCOME) {
                bool isPrintable = (event.text.unicode < 128 && event.text.unicode >= 32);
                bool notTooLong = (playerName.length() < 15); 
                
                if(isPrintable && notTooLong) {
                    playerName += static_cast<char>(event.text.unicode);
                }
                else if (event.text.unicode == 8 && !playerName.empty()) { 
                    playerName.pop_back(); 
                }
            }
        }
    }
}

void Game::updateGame() {
    if(currentState == GENERATING) {
        for(int i = 0; i < 5; i++) maze->step();
        if(!maze->isGenerating()) { 
            player = new Player(maze->getStartX(), maze->getStartY(), CELL_SIZE, 3.0f); 
            createRiddles();
            spawnEnemies();
            currentState = PLAYING; 
            timer.restart(); 
        }
    }
    if(currentState == PLAYING) {
        elapsedTime = timer.getElapsedTime().asSeconds();
        player->updateBullets(COLS, ROWS);
        updateEnemies();
        checkEnemyCollisions();
        checkBulletCollisions();
    
        if(player->getHealth() <= 0) {
            currentState = GAME_OVER;
        }
    }
}

void Game::draw() {
    window.clear(sf::Color(10, 10, 20));
    if(currentState == WELCOME) showWelcomeScreen();
    else if(currentState == LEADERBOARD_VIEW) showLeaderboard();
    else if(currentState == GENERATING) maze->draw(window);
    else if(currentState == PLAYING || currentState == RIDDLE_ACTIVE) {
        sf::RectangleShape darkness(sf::Vector2f(MAZE_WIDTH, MAZE_HEIGHT)); 
        darkness.setFillColor(sf::Color(0, 0, 0, 220)); 
        window.draw(darkness);
        
        maze->drawWithVision(window, *player);
        
        for(const auto& e : enemies) {
            bool canSeeEnemy = player->isInVision(e->getX(), e->getY());
            if(canSeeEnemy) e->draw(window);
        }
        
        showRiddleMarkers();
        player->draw(window);
        
        float visionSize = player->getVisionRadius() * CELL_SIZE;
        sf::CircleShape visionCircle(visionSize); 
        visionCircle.setOrigin(visionSize, visionSize);
        visionCircle.setPosition(player->getCellX() * CELL_SIZE + CELL_SIZE / 2, player->getCellY() * CELL_SIZE + CELL_SIZE / 2);
        visionCircle.setFillColor(sf::Color(255, 255, 255, 5)); 
        visionCircle.setOutlineColor(sf::Color(255, 255, 150, 30)); 
        visionCircle.setOutlineThickness(2); 
        window.draw(visionCircle);
        
        showGameInfo(); 
        showMiniLeaderboard();
        showHealthBar();
        
        if(currentState == RIDDLE_ACTIVE) showRiddleBox();
    } else if(currentState == VICTORY) showVictoryScreen();
    else if(currentState == GAME_OVER) showGameOverScreen();
    
    window.display();
}

void Game::run() {
    while(window.isOpen()) { 
        handleInput(); 
        updateGame(); 
        draw(); 
    }
}