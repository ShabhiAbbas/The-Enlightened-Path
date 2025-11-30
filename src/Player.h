#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct Bullet {
    float x, y;
    int dirX, dirY;  // Direction: -1, 0, or 1
    float speed;
    bool active;
    
    Bullet(float px, float py, int dx, int dy, float s)
        : x(px), y(py), dirX(dx), dirY(dy), speed(s), active(true) {}
};

class Player : public MovableEntity {
private:
    int cellSize;
    float visionRadius;
    sf::Clock animClock;
    float health;
    float maxHealth;
    bool isInvisible;
    bool canKillEnemies;
    int deathCount;
    int initialX;
    int initialY;
    int ammo;
    std::vector<Bullet> bullets;
    
public:
    Player(int startX, int startY, int cellSize_, float visionRadius_ = 3.0f);
    virtual ~Player();
    int getCellX() const;
    int getCellY() const;
    float getVisionRadius() const;
    float getHealth() const;
    float getMaxHealth() const;
    bool getIsInvisible() const;
    bool getCanKillEnemies() const;
    int getDeathCount() const;
    int getAmmo() const;
    std::vector<Bullet>& getBullets();
    void increaseHealth(float amount);
    void increaseVision(float amount);
    void addAmmo(int amount);
    void fire();
    void updateBullets(int mazeCols, int mazeRows);
    void takeDamage(float amount);
    void respawn();
    void setInvisible(bool inv);
    void setCanKillEnemies(bool canKill);
    virtual void move(int dx, int dy) override;
    bool isInVision(int cellX, int cellY) const;
    virtual void draw(sf::RenderWindow& window) const override;
};

#endif // PLAYER_H
