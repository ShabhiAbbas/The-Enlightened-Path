#include "Riddle.h"

// Riddle base class: stores a question/answer pair and a position with reward type.
// When unsolved, a riddle renders a small marker on the maze; solving
// the riddle grants a reward (see getRewardType()).
Riddle::Riddle(const std::string& q, const std::string& a, int posX, int posY, int cs, RiddleRewardType rt)
    : GameObject(posX, posY), question(q), answer(a), solved(false), cellSize(cs), rewardType(rt) {}

Riddle::~Riddle() {}

std::string Riddle::getQuestion() const { return question; }
std::string Riddle::getAnswer() const { return answer; }
bool Riddle::isSolved() const { return solved; }
void Riddle::setSolved(bool s) { solved = s; }
RiddleRewardType Riddle::getRewardType() const { return rewardType; }

// Default reward (vision increase)
float Riddle::getReward() const { return 2.0f; }

// Default marker color for unsolved riddles (orange)
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

// EasyRiddle: smaller reward, different color based on type
EasyRiddle::EasyRiddle(const std::string& q,const std::string& a,int posX,int posY,int cs,RiddleRewardType rt)
    : Riddle(q,a,posX,posY,cs,rt) {}
float EasyRiddle::getReward() const { return 1.5f; }
sf::Color EasyRiddle::getMarkerColor() const { 
    // Color depends on reward type
    if(rewardType == INVISIBILITY_REWARD) return sf::Color(100, 200, 255); // Blue
    if(rewardType == KILL_POWER_REWARD) return sf::Color(255, 255, 0); // Yellow
    if(rewardType == HEALTH_REWARD) return sf::Color(100, 255, 180); // Teal for health
    return sf::Color(100, 255, 100); // Green for vision
}

// HardRiddle: larger reward, different color based on type
HardRiddle::HardRiddle(const std::string& q,const std::string& a,int posX,int posY,int cs,RiddleRewardType rt)
    : Riddle(q,a,posX,posY,cs,rt) {}
float HardRiddle::getReward() const { return 3.0f; }
sf::Color HardRiddle::getMarkerColor() const { 
    // Color depends on reward type
    if(rewardType == INVISIBILITY_REWARD) return sf::Color(150, 100, 255); // Purple
    if(rewardType == KILL_POWER_REWARD) return sf::Color(255, 150, 0); // Orange-red
    if(rewardType == HEALTH_REWARD) return sf::Color(200, 255, 180); // Light green for health
    return sf::Color(255, 100, 100); // Red for vision
}
