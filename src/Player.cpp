#include "Player.h"
#include "Maze.h"
#include <algorithm>
#include <cmath>

Player::Player(int startX, int startY, int cellSize_, float visionRadius_): MovableEntity(startX, startY), cellSize(cellSize_), visionRadius(visionRadius_), 
    health(3.0f), maxHealth(3.0f), isInvisible(false), canKillEnemies(false), 
    deathCount(0), initialX(startX), initialY(startY), ammo(0) {
        damageCooldown.restart(); 
    }

Player::~Player() {}

int Player::getCellX() const { return static_cast<int>(x); }
int Player::getCellY() const { return static_cast<int>(y); }

float Player::getVisionRadius() const { return visionRadius; }
float Player::getHealth() const { return health; }
float Player::getMaxHealth() const { return maxHealth; }

bool Player::getIsInvisible() const { 
    if (!isInvisible) return false;
    if (invisibilityTimer.getElapsedTime().asSeconds() > GameConstants::INVISIBILITY_DURATION) {
        return false; 
    }
    return true; 
}

void Player::setInvisible(bool inv) { 
    isInvisible = inv;
    if (inv) {
        invisibilityTimer.restart();
    }
}

bool Player::getCanKillEnemies() const { return canKillEnemies; }
int Player::getDeathCount() const { return deathCount; }
int Player::getAmmo() const { return ammo; }
std::vector<Bullet>& Player::getBullets() { return bullets; }

void Player::increaseHealth(float amount) {
    health += amount;
    if(health > maxHealth) health = maxHealth;
}

void Player::increaseVision(float amount) { 
    visionRadius += amount; 
    if(visionRadius > 15.0f) visionRadius = 15.0f; 
}

void Player::addAmmo(int amount) {
    ammo += amount;
}

void Player::fire() {
    if(ammo <= 0) return;
    int dx = 0, dy = 0;
    if(direction == 0) dy = -1;
    else if(direction == 1) dx = 1;
    else if(direction == 2) dy = 1;
    else if(direction == 3) dx = -1;
    bullets.emplace_back(x, y, dx, dy, 0.5f);
    ammo--;
}


void Player::updateBullets(int mazeCols, int mazeRows, const Maze* maze) {
    for(auto& bullet : bullets) {
        if(!bullet.active) continue;
        int cx = static_cast<int>(std::round(bullet.x));
        int cy = static_cast<int>(std::round(bullet.y));

        if (maze && !maze->canMove(cx, cy, bullet.dirX, bullet.dirY)) {
            float nextX = bullet.x + bullet.dirX * bullet.speed;
            float nextY = bullet.y + bullet.dirY * bullet.speed;
            
            int nextCx = static_cast<int>(std::round(nextX));
            int nextCy = static_cast<int>(std::round(nextY));
            
            if (nextCx != cx || nextCy != cy) {
                if (!maze->canMove(cx, cy, nextCx - cx, nextCy - cy)){
                    bullet.active = false;
                    continue;
                }
            }
            
            bullet.x = nextX;
            bullet.y = nextY;
        } else {

            float nextX = bullet.x + bullet.dirX * bullet.speed;
            float nextY = bullet.y + bullet.dirY * bullet.speed;
            
            int cx = static_cast<int>(std::round(bullet.x));
            int cy = static_cast<int>(std::round(bullet.y));
            int nextCx = static_cast<int>(std::round(nextX));
            int nextCy = static_cast<int>(std::round(nextY));

            if (nextCx != cx || nextCy != cy) {
                if (maze && !maze->canMove(cx, cy, nextCx - cx, nextCy - cy)) {
                    bullet.active = false; 
                    continue;
                }
            }
            bullet.x = nextX;
            bullet.y = nextY;
        }

        if(bullet.x < 0 || bullet.x >= mazeCols || bullet.y < 0 || bullet.y >= mazeRows) {
            bullet.active = false;
        }
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.active; }), bullets.end());
}

void Player::takeDamage(float amount) {
    if(damageCooldown.getElapsedTime().asSeconds() < 1.0f) return;
    health -= amount;
    damageCooldown.restart(); 
    x = static_cast<float>(initialX);
    y = static_cast<float>(initialY);
    if(health < 0) health = 0;
}

void Player::respawn() {
    x = static_cast<float>(initialX);
    y = static_cast<float>(initialY);
    health = maxHealth;
    isInvisible = false;
    canKillEnemies = false;
    ammo = 0;
    bullets.clear();
    damageCooldown.restart();
}

void Player::setCanKillEnemies(bool canKill) { canKillEnemies = canKill; }
void Player::move(int dx, int dy) { MovableEntity::move(dx, dy); }
bool Player::isInVision(int cellX, int cellY) const { 
    float dx = cellX - x; 
    float dy = cellY - y; 
    return (dx*dx + dy*dy) <= (visionRadius * visionRadius); 
}

void Player::drawAimArrow(sf::RenderWindow& window, int cellSize) const {
    if(ammo <= 0) return;
    
    float px = x * cellSize + cellSize / 2;
    float py = y * cellSize + cellSize / 2;
    
    // Calculate arrow direction based on current facing direction
    float arrowLength = cellSize * 2.5f;
    float dx = 0, dy = 0;
    
    if(direction == 0) dy = -arrowLength;       // Up
    else if(direction == 1) dx = arrowLength;   // Right
    else if(direction == 2) dy = arrowLength;   // Down
    else if(direction == 3) dx = -arrowLength;  // Left
    
    // Draw arrow line
    sf::Vertex arrowLine[] = {
        sf::Vertex(sf::Vector2f(px, py), sf::Color(255, 100, 100)),
        sf::Vertex(sf::Vector2f(px + dx, py + dy), sf::Color(255, 100, 100))
    };
    window.draw(arrowLine, 2, sf::Lines);
    
    // Draw arrow head (triangle)
    float arrowHeadSize = cellSize * 0.4f;
    float perpX = -dy / arrowLength * arrowHeadSize;
    float perpY = dx / arrowLength * arrowHeadSize;
    
    sf::Vector2f tipPos(px + dx, py + dy);
    sf::Vector2f leftBase(px + dx - dx * 0.2f - perpX, py + dy - dy * 0.2f - perpY);
    sf::Vector2f rightBase(px + dx - dx * 0.2f + perpX, py + dy - dy * 0.2f + perpY);
    
    sf::Vertex arrowHead[] = {
        sf::Vertex(tipPos, sf::Color(255, 100, 100)),
        sf::Vertex(leftBase, sf::Color(255, 100, 100)),
        sf::Vertex(rightBase, sf::Color(255, 100, 100)),
        sf::Vertex(tipPos, sf::Color(255, 100, 100))
    };
    window.draw(arrowHead, 4, sf::LineStrip);
}

void Player::draw(sf::RenderWindow& window) const {
    float px = x * cellSize + cellSize / 2;
    float py = y * cellSize + cellSize / 2;
    
    sf::CircleShape playerCircle(cellSize / 3);
    playerCircle.setOrigin(cellSize / 3, cellSize / 3);
    playerCircle.setPosition(px, py);
    
    if(getIsInvisible()) {
        playerCircle.setFillColor(sf::Color(100, 100, 255, 150));
    } else {
        if(health > 2.0f) playerCircle.setFillColor(sf::Color(100, 255, 100));
        else if(health > 1.0f) playerCircle.setFillColor(sf::Color(255, 255, 100));
        else playerCircle.setFillColor(sf::Color(255, 100, 100));
    }
    
    window.draw(playerCircle);
    
    sf::CircleShape eye(2);
    eye.setFillColor(sf::Color(0, 0, 0));
    eye.setPosition(px - cellSize / 6, py - cellSize / 6);
    window.draw(eye);
    eye.setPosition(px + cellSize / 6, py - cellSize / 6);
    window.draw(eye);
    
    if(health > 2.0f) {
        sf::VertexArray smile(sf::LinesStrip, 4);
        smile[0].position = sf::Vector2f(px - cellSize/6, py + cellSize/8);
        smile[1].position = sf::Vector2f(px - cellSize/6, py + cellSize/4);
        smile[2].position = sf::Vector2f(px + cellSize/6, py + cellSize/4);
        smile[3].position = sf::Vector2f(px + cellSize/6, py + cellSize/8);
        for(int i=0; i<4; ++i) smile[i].color = sf::Color::Black;
        window.draw(smile);
    } else if(health > 1.0f) {
        sf::RectangleShape mouth(sf::Vector2f(cellSize / 3, 2));
        mouth.setPosition(px - cellSize / 6, py + cellSize / 6);
        mouth.setFillColor(sf::Color(0, 0, 0));
        window.draw(mouth);
    } else {
        sf::VertexArray frown(sf::LinesStrip, 4);
        frown[0].position = sf::Vector2f(px - cellSize/6, py + cellSize/4);
        frown[1].position = sf::Vector2f(px - cellSize/6, py + cellSize/8);
        frown[2].position = sf::Vector2f(px + cellSize/6, py + cellSize/8);
        frown[3].position = sf::Vector2f(px + cellSize/6, py + cellSize/4);
        for(int i=0; i<4; ++i) frown[i].color = sf::Color::Black;
        window.draw(frown);
    }
    
    for(const auto& bullet : bullets) {
        sf::RectangleShape bulletShape(sf::Vector2f(cellSize * 0.3f, cellSize * 0.3f));
        bulletShape.setOrigin(cellSize * 0.15f, cellSize * 0.15f);
        bulletShape.setPosition(bullet.x * cellSize + cellSize / 2, bullet.y * cellSize + cellSize / 2);
        bulletShape.setFillColor(sf::Color(255, 255, 255));
        window.draw(bulletShape);
    }
}