#include "GameObject.h"
#include "Player.h"
#include <cmath>

// Simple base implementation for game objects and movable entities.
// GameObject stores position and provides overridable update/draw hooks.
// MovableEntity adds simple grid-relative movement and facing direction.

GameObject::GameObject(float posX, float posY) : x(posX), y(posY) {}
GameObject::~GameObject() {}
void GameObject::update(float deltaTime) { (void)deltaTime; }
float GameObject::getX() const { return x; }
float GameObject::getY() const { return y; }
void GameObject::setPosition(float posX, float posY) { x = posX; y = posY; }

MovableEntity::MovableEntity(float posX, float posY, float speed_) : GameObject(posX, posY), direction(2), speed(speed_) {}
MovableEntity::~MovableEntity() {}
// move: change position based on `xpos`/`ypos` and `speed`.
// `direction` is updated to reflect the last movement direction.
void MovableEntity::move(int xpos, int ypos) {
    x += xpos * speed;
    y += ypos * speed;
    if(xpos < 0) direction = 3;
    else if(xpos > 0) direction = 1;
    else if(ypos < 0) direction = 0;
    else if(ypos > 0) direction = 2;
}
int MovableEntity::getDirection() const { return direction; }
