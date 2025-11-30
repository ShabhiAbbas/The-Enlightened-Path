#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>

class GameObject {
protected:
    float x, y;
public:
    GameObject(float posX = 0, float posY = 0);
    virtual ~GameObject();
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual void update(float deltaTime);
    float getX() const;
    float getY() const;
    void setPosition(float posX, float posY);
};

class MovableEntity : public GameObject {
protected:
    int direction; // 0=up,1=right,2=down,3=left
    float speed;
public:
    MovableEntity(float posX = 0, float posY = 0, float speed = 1.0f);
    virtual ~MovableEntity();
    virtual void move(int xpos, int ypos);
    int getDirection() const;
};

#endif // GAMEOBJECT_H
