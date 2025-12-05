#ifndef RIDDLE_H
#define RIDDLE_H

#include "GameObject.h"
#include <string>
#include <SFML/Graphics.hpp>


enum RiddleRewardType { VISION_REWARD, INVISIBILITY_REWARD, KILL_POWER_REWARD, HEALTH_REWARD };

class Riddle : public GameObject {
protected:
    std::string question;
    std::string answer;
    bool solved;
    int cellSize;
    RiddleRewardType rewardType;
    virtual sf::Color getMarkerColor() const;
public:
    Riddle(const std::string& q = "", const std::string& a = "", int posX = 0, int posY = 0, int cs = 30, RiddleRewardType rt = VISION_REWARD);
    virtual ~Riddle();
    std::string getQuestion() const;
    std::string getAnswer() const;
    bool isSolved() const;
    void setSolved(bool s);
    RiddleRewardType getRewardType() const;
    virtual float getReward() const;
    virtual void draw(sf::RenderWindow& window) const override;
};

class EasyRiddle : public Riddle {
public:
    EasyRiddle(const std::string& q, const std::string& a, int posX, int posY, int cs, RiddleRewardType rt = VISION_REWARD);
    virtual float getReward() const override;
protected:
    virtual sf::Color getMarkerColor() const override;
};

class HardRiddle : public Riddle {
public:
    HardRiddle(const std::string& q, const std::string& a, int posX, int posY, int cs, RiddleRewardType rt = VISION_REWARD);
    virtual float getReward() const override;
protected:
    virtual sf::Color getMarkerColor() const override;
};

#endif 
