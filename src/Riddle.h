#ifndef RIDDLE_H
#define RIDDLE_H

#include "GameObject.h"
#include <string>
#include <SFML/Graphics.hpp>

class Riddle : public GameObject {
protected:
    std::string question;
    std::string answer;
    bool solved;
    int cellSize;
    virtual sf::Color getMarkerColor() const;
public:
    Riddle(const std::string& q = "", const std::string& a = "", int posX = 0, int posY = 0, int cs = 30);
    virtual ~Riddle();
    std::string getQuestion() const;
    std::string getAnswer() const;
    bool isSolved() const;
    void setSolved(bool s);
    virtual float getReward() const;
    virtual void draw(sf::RenderWindow& window) const override;
};

class EasyRiddle : public Riddle {
public:
    EasyRiddle(const std::string& q,const std::string& a,int posX,int posY,int cs);
    virtual float getReward() const override;
protected:
    virtual sf::Color getMarkerColor() const override;
};

class HardRiddle : public Riddle {
public:
    HardRiddle(const std::string& q,const std::string& a,int posX,int posY,int cs);
    virtual float getReward() const override;
protected:
    virtual sf::Color getMarkerColor() const override;
};

#endif // RIDDLE_H
