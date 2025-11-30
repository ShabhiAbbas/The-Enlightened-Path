#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>

class Player : public MovableEntity {
private:
    int cellSize;
    float visionRadius;
    sf::Clock animClock;
public:
    Player(int startX, int startY, int cellSize_, float visionRadius_ = 3.0f);
    virtual ~Player();
    int getCellX() const;
    int getCellY() const;
    float getVisionRadius() const;
    void increaseVision(float amount);
    virtual void move(int dx, int dy) override;
    bool isInVision(int cellX, int cellY) const;
    virtual void draw(sf::RenderWindow& window) const override;
};

#endif // PLAYER_H
