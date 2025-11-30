#include "Player.h"

// Player represents the user-controlled entity in the maze.
// It keeps a vision radius (in cells) which is used to limit rendering
// to nearby cells (fog-of-war style).

Player::Player(int startX, int startY, int cellSize_, float visionRadius_) : MovableEntity(startX, startY), cellSize(cellSize_), visionRadius(visionRadius_) {}
Player::~Player() {}

// getCellX / getCellY: return integer cell coordinates derived from
// the internal float position `x`/`y`.
int Player::getCellX() const { return static_cast<int>(x); }
int Player::getCellY() const { return static_cast<int>(y); }

float Player::getVisionRadius() const { return visionRadius; }

// increaseVision: expand how far the player can see (bounded maximum)
void Player::increaseVision(float amount) { visionRadius += amount; if(visionRadius > 15.0f) visionRadius = 15.0f; }

// move: delegate to MovableEntity, which handles updating position and direction
void Player::move(int dx, int dy) { MovableEntity::move(dx, dy); }

// isInVision: simple circular check using squared distance for efficiency
bool Player::isInVision(int cellX, int cellY) const { float dx = cellX - x; float dy = cellY - y; return (dx*dx + dy*dy) <= (visionRadius * visionRadius); }

// draw: draw a small triangular marker representing the player at the
// center of the current cell. Rotation indicates facing direction.
void Player::draw(sf::RenderWindow& window) const {
    float px = x * cellSize + cellSize / 2;
    float py = y * cellSize + cellSize / 2;
    sf::CircleShape player(cellSize / 3, 3);
    player.setOrigin(cellSize / 3, cellSize / 3);
    player.setPosition(px, py);
    player.setRotation(direction * 90 + 90);
    player.setFillColor(sf::Color(255,220,100));
    window.draw(player);
}
