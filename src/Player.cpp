#include "Player.h"
#include <algorithm>

// Player represents the user-controlled entity in the maze.
// It keeps a vision radius (in cells) which is used to limit rendering
// to nearby cells (fog-of-war style). Also tracks health and special abilities.
// Now includes a gun with ammo and bullets for the kill power reward.

Player::Player(int startX, int startY, int cellSize_, float visionRadius_) 
    : MovableEntity(startX, startY), cellSize(cellSize_), visionRadius(visionRadius_), 
    health(3.0f), maxHealth(3.0f), isInvisible(false), canKillEnemies(false), 
    deathCount(0), initialX(startX), initialY(startY), ammo(0) {}
Player::~Player() {}

// getCellX / getCellY: return integer cell coordinates derived from
// the internal float position `x`/`y`.
int Player::getCellX() const { return static_cast<int>(x); }
int Player::getCellY() const { return static_cast<int>(y); }

float Player::getVisionRadius() const { return visionRadius; }
float Player::getHealth() const { return health; }
float Player::getMaxHealth() const { return maxHealth; }
bool Player::getIsInvisible() const { return isInvisible; }
bool Player::getCanKillEnemies() const { return canKillEnemies; }
int Player::getDeathCount() const { return deathCount; }
int Player::getAmmo() const { return ammo; }
std::vector<Bullet>& Player::getBullets() { return bullets; }

// increaseVision: expand how far the player can see (bounded maximum)
void Player::increaseVision(float amount) { 
    visionRadius += amount; 
    if(visionRadius > 15.0f) visionRadius = 15.0f; 
}

void Player::addAmmo(int amount) {
    ammo += amount;
}

void Player::fire() {
    if(ammo <= 0) return;
    
    // Fire a bullet in the direction the player is facing
    // direction: 0=up, 1=right, 2=down, 3=left
    int dx = 0, dy = 0;
    if(direction == 0) dy = -1;
    else if(direction == 1) dx = 1;
    else if(direction == 2) dy = 1;
    else if(direction == 3) dx = -1;
    
    bullets.emplace_back(x, y, dx, dy, 0.5f);
    ammo--;
}

void Player::updateBullets(int mazeCols, int mazeRows) {
    for(auto& bullet : bullets) {
        if(!bullet.active) continue;
        
        // Move bullet
        bullet.x += bullet.dirX * bullet.speed;
        bullet.y += bullet.dirY * bullet.speed;
        
        // Check if bullet is out of bounds
        if(bullet.x < 0 || bullet.x >= mazeCols || bullet.y < 0 || bullet.y >= mazeRows) {
            bullet.active = false;
        }
    }
    
    // Remove inactive bullets
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(), 
                       [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
}

void Player::takeDamage(float amount) {
    health -= amount;
    if(health < 0) health = 0;
    if(health <= 0) deathCount++;
}

void Player::respawn() {
    x = static_cast<float>(initialX);
    y = static_cast<float>(initialY);
    health = maxHealth;
    isInvisible = false;
    canKillEnemies = false;
    ammo = 0;
    bullets.clear();
}

void Player::increaseHealth(float amount) {
    health += amount;
    if(health > maxHealth) health = maxHealth;
}

void Player::setInvisible(bool inv) { isInvisible = inv; }
void Player::setCanKillEnemies(bool canKill) { canKillEnemies = canKill; }

// move: delegate to MovableEntity, which handles updating position and direction
void Player::move(int dx, int dy) { MovableEntity::move(dx, dy); }

// isInVision: simple circular check using squared distance for efficiency
bool Player::isInVision(int cellX, int cellY) const { 
    float dx = cellX - x; 
    float dy = cellY - y; 
    return (dx*dx + dy*dy) <= (visionRadius * visionRadius); 
}

// draw: draw player as emoji face. Emoji changes based on health:
// - 3 health (happy face): 😊
// - 2 health (normal face): 😐
// - 1 health (sad face): 😞
// Also draw bullets as small white squares
void Player::draw(sf::RenderWindow& window) const {
    float px = x * cellSize + cellSize / 2;
    float py = y * cellSize + cellSize / 2;
    
    // Draw player as a circle with color representing emotion
    sf::CircleShape playerCircle(cellSize / 3);
    playerCircle.setOrigin(cellSize / 3, cellSize / 3);
    playerCircle.setPosition(px, py);
    
    if(isInvisible) {
        playerCircle.setFillColor(sf::Color(100, 100, 255, 150));
    } else {
        if(health >= 3.0f) {
            playerCircle.setFillColor(sf::Color(100, 255, 100));  // Green - happy
        } else if(health >= 2.0f) {
            playerCircle.setFillColor(sf::Color(255, 255, 100));  // Yellow - normal
        } else {
            playerCircle.setFillColor(sf::Color(255, 100, 100));  // Red - sad
        }
    }
    
    // Draw player circle
    window.draw(playerCircle);
    
    // Draw face dots for visual indication
    sf::CircleShape eye(2);
    eye.setFillColor(sf::Color(0, 0, 0));
    
    // Left eye
    eye.setPosition(px - cellSize / 6, py - cellSize / 6);
    window.draw(eye);
    
    // Right eye
    eye.setPosition(px + cellSize / 6, py - cellSize / 6);
    window.draw(eye);
    
    // Draw mouth based on health
    if(health >= 2.0f) {
        // Happy/Normal: horizontal line
        sf::RectangleShape mouth(sf::Vector2f(cellSize / 4, 1));
        mouth.setPosition(px - cellSize / 8, py + cellSize / 8);
        mouth.setFillColor(sf::Color(0, 0, 0));
        window.draw(mouth);
    } else {
        // Sad: inverted arc (use two small lines)
        sf::CircleShape mouthArc(cellSize / 4);
        mouthArc.setOrigin(cellSize / 4, cellSize / 4);
        mouthArc.setPosition(px, py + cellSize / 6);
        mouthArc.setFillColor(sf::Color::Transparent);
        mouthArc.setOutlineColor(sf::Color(0, 0, 0));
        mouthArc.setOutlineThickness(1);
        window.draw(mouthArc);
    }
    
    // Draw bullets
    for(const auto& bullet : bullets) {
        sf::RectangleShape bulletShape(sf::Vector2f(cellSize * 0.3f, cellSize * 0.3f));
        bulletShape.setOrigin(cellSize * 0.15f, cellSize * 0.15f);
        bulletShape.setPosition(bullet.x * cellSize + cellSize / 2, bullet.y * cellSize + cellSize / 2);
        bulletShape.setFillColor(sf::Color(255, 255, 255));
        window.draw(bulletShape);
    }
}

