#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>


class Player;
class Maze;

class Enemy : public MovableEntity {
private:

    int cellSize;
    float detectionRadius;
    float health;
    bool isDead;

public:
   
    Enemy(int startX, int startY, int cellSize_, float detectionRadius_ = 4.0f);
    virtual ~Enemy();

    float getDetectionRadius() const;
    float getHealth() const;
    bool dead() const;

    void takeDamage(float amount);

    void moveTowardPlayer(const Player& player, int mazeCol, int mazeRow);
    void moveTowardPlayer(const Player& player, int mazeCol, int mazeRow,const Maze* maze);

    bool isInDetectionRange(const Player& player) const;


    virtual void draw(sf::RenderWindow& window) const override;
};

#endif 
