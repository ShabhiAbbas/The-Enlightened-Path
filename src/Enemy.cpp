#include "Enemy.h"
#include "Player.h"
#include "Maze.h"
#include <cmath>

// Enemy: AI-controlled entity that detects and chases the player
// Spawns at random locations and follows player when within detection radius
Enemy::Enemy(int startX, int startY, int cellSize_, float detectionRadius_)
    : MovableEntity(startX, startY, 0.05f), cellSize(cellSize_), detectionRadius(detectionRadius_), health(3.0f), isDead(false) {}

Enemy::~Enemy() {}

float Enemy::getDetectionRadius() const { return detectionRadius; }
float Enemy::getHealth() const { return health; }
bool Enemy::dead() const { return isDead; }

void Enemy::takeDamage(float amount) {
    health -= amount;
    if(health <= 0) isDead = true;
}

bool Enemy::isInDetectionRange(const Player& player) const {
    float dx = player.getX() - x;
    float dy = player.getY() - y;
    return (dx * dx + dy * dy) <= (detectionRadius * detectionRadius);
}

void Enemy::moveTowardPlayer(const Player& player, int mazeCol, int mazeRow, const Maze* maze) {
    float targetX = player.getX();
    float targetY = player.getY();

    float dx = targetX - x;
    float dy = targetY - y;

    // Round to nearest cell for wall collision checking
    int currCellX = static_cast<int>(std::round(x));
    int currCellY = static_cast<int>(std::round(y));

    int stepX = 0;
    int stepY = 0;
    const float threshold = 0.1f;

    if(std::abs(dx) > std::abs(dy)) {
        if(dx > threshold) stepX = 1;
        else if(dx < -threshold) stepX = -1;
    } else {
        if(dy > threshold) stepY = 1;
        else if(dy < -threshold) stepY = -1;
    }

    // Try primary axis move if wall check passes
    if(stepX != 0 || stepY != 0) {
        if(maze && !maze->canMove(currCellX, currCellY, stepX, stepY)) {
            // Primary direction blocked, try fallback
            if(stepX != 0) {
                int altStepY = (dy > threshold) ? 1 : ((dy < -threshold) ? -1 : 0);
                if(altStepY != 0 && maze->canMove(currCellX, currCellY, 0, altStepY)) {
                    move(0, altStepY);
                    return;
                }
            } else if(stepY != 0) {
                int altStepX = (dx > threshold) ? 1 : ((dx < -threshold) ? -1 : 0);
                if(altStepX != 0 && maze->canMove(currCellX, currCellY, altStepX, 0)) {
                    move(altStepX, 0);
                    return;
                }
            }
            // No fallback available, don't move
            return;
        }
        // Primary direction is clear, move
        move(stepX, stepY);
    }
}

// Backwards-compatible overload: call the new function without maze
void Enemy::moveTowardPlayer(const Player& player, int mazeCol, int mazeRow) {
    moveTowardPlayer(player, mazeCol, mazeRow, nullptr);
}

void Enemy::draw(sf::RenderWindow& window) const {
    if(!isDead) {
        float px = x * cellSize + cellSize / 2;
        float py = y * cellSize + cellSize / 2;
        
        // Draw enemy as a circle (red for angry)
        sf::CircleShape enemyCircle(cellSize / 3);
        enemyCircle.setOrigin(cellSize / 3, cellSize / 3);
        enemyCircle.setPosition(px, py);
        enemyCircle.setFillColor(sf::Color(255, 50, 50));  // Red
        window.draw(enemyCircle);
        
        // Draw angry face with eyes and mouth
        sf::CircleShape eye(2);
        eye.setFillColor(sf::Color(255, 255, 255));
        
        // Left eye
        eye.setPosition(px - cellSize / 6, py - cellSize / 6);
        window.draw(eye);
        
        // Right eye
        eye.setPosition(px + cellSize / 6, py - cellSize / 6);
        window.draw(eye);
        
        // Angry mouth: downward line
        sf::RectangleShape mouth(sf::Vector2f(cellSize / 4, 2));
        mouth.setPosition(px - cellSize / 8, py + cellSize / 8);
        mouth.setFillColor(sf::Color(255, 255, 255));
        window.draw(mouth);
    }
}
