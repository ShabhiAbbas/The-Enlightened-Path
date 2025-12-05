#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Maze; 

struct Bullet {
    float x, y;
    int dirX, dirY;
    float speed;
    bool active;

    Bullet(float px, float py, int dx, int dy, float s):x(px), y(py), dirX(dx), dirY(dy), speed(s), active(true) {}
};

class Player : public MovableEntity{
private:
    int cellSize;
    float visionRadius;
    
    sf::Clock damageCooldown; 
    sf::Clock invisibilityTimer;
    sf::Clock invisibilityClock;

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
    
 
    void updateBullets(int mazeCols, int mazeRows, const Maze* maze);
    
    void takeDamage(float amount);
    void respawn();
    
    void setInvisible(bool inv);
    void setCanKillEnemies(bool canKill);
    
    virtual void move(int dx, int dy) override;
    bool isInVision(int cellX, int cellY) const;
    virtual void draw(sf::RenderWindow& window) const override;
};


namespace GameConstants {
    constexpr float INVISIBILITY_DURATION = 10.0f;
    constexpr int KILL_POWER_AMMO_REWARD = 6;
}

#endif