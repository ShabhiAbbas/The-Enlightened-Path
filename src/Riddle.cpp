#include "Riddle.h"

// Riddle base class: stores a question/answer pair and a position.
// When unsolved, a riddle renders a small marker on the maze; solving
// the riddle grants a vision reward to the player (see getReward()).
Riddle::Riddle(const std::string& q, const std::string& a, int posX, int posY, int cs)
    : GameObject(posX, posY), question(q), answer(a), solved(false), cellSize(cs) {}

Riddle::~Riddle() {}

std::string Riddle::getQuestion() const { return question; }
std::string Riddle::getAnswer() const { return answer; }
bool Riddle::isSolved() const { return solved; }
void Riddle::setSolved(bool s) { solved = s; }

// Default reward (how much vision is gained when solved)
float Riddle::getReward() const { return 2.0f; }

// Default marker color for unsolved riddles
sf::Color Riddle::getMarkerColor() const { return sf::Color(255,200,50); }

void Riddle::draw(sf::RenderWindow& window) const {
    if(!solved) {
        // Draw a small hexagonal marker centered in the cell
        float px = x * cellSize + cellSize / 2;
        float py = y * cellSize + cellSize / 2;
        sf::CircleShape marker(8,6);
        marker.setOrigin(8,8);
        marker.setPosition(px, py);
        marker.setFillColor(getMarkerColor());
        marker.setOutlineColor(sf::Color(255,255,100));
        marker.setOutlineThickness(2);
        window.draw(marker);
    }
}

// EasyRiddle: smaller reward, different color
EasyRiddle::EasyRiddle(const std::string& q,const std::string& a,int posX,int posY,int cs)
    : Riddle(q,a,posX,posY,cs) {}
float EasyRiddle::getReward() const { return 1.5f; }
sf::Color EasyRiddle::getMarkerColor() const { return sf::Color(100,255,100); }

// HardRiddle: larger reward, different color
HardRiddle::HardRiddle(const std::string& q,const std::string& a,int posX,int posY,int cs)
    : Riddle(q,a,posX,posY,cs) {}
float HardRiddle::getReward() const { return 3.0f; }
sf::Color HardRiddle::getMarkerColor() const { return sf::Color(255,100,100); }
