#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>

// Forward declarations
class Player;
class Maze;

class Enemy : public MovableEntity {
private:
    // Size of a cell (pixels) used for drawing and position calculations
    int cellSize;

    // How far (in cells) the enemy can detect the player
    float detectionRadius;

    // Enemy health; when <= 0 the enemy is considered dead
    float health;

    // Simple dead flag for convenience
    bool isDead;

public:
    // Construct an enemy at a grid cell position
    Enemy(int startX, int startY, int cellSize_, float detectionRadius_ = 4.0f);
    virtual ~Enemy();

    // Accessors
    float getDetectionRadius() const;
    float getHealth() const;
    bool dead() const;

    // Modify health
    void takeDamage(float amount);

    // Movement helpers: chase player, optionally using maze for pathing
    void moveTowardPlayer(const Player& player, int mazeCol, int mazeRow);
    void moveTowardPlayer(const Player& player, int mazeCol, int mazeRow,
                         const Maze* maze);

    // Simple proximity check against player
    bool isInDetectionRange(const Player& player) const;

    // Draw the enemy
    virtual void draw(sf::RenderWindow& window) const override;
};

#endif // ENEMY_H
