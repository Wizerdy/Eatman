#include "Bullet.h"

Bullet::Bullet(float posX, float posY, float size, float speed) : Character(posX, posY, size, speed) {
    isActive = true;

    rect.setOrigin(size, 0);
    rect.setSize(Vector2f(size * 2.2f, size * 2.4f));
    //rect.setOutlineThickness(0);
    shape.setOutlineThickness(0);

    SetShapeColor(Color(CHARCOLOR.r, 20, 20, CHARCOLOR.a));
}

Bullet::~Bullet() {
    
}

void Bullet::Move(float deltaTime) {
    //SetPos(posX + dirX * speed * deltaTime, posY + dirY * speed * deltaTime);
    Character::Move(deltaTime);
    SetPos(GetPosX(), GetPosY());
}

void Bullet::SetPos(float posX, float posY) {
    Character::SetPos(posX, posY);
    rect.setPosition(posX, posY);
}

void Bullet::SetDir(float x, float y) {
    Character::SetDir(x, y);

    float ang = atan(y / x) * RADTODEG;;
    if (y < 0) {
        
    }
    if (x < 0) {
        ang += 180;
    }

    rect.setRotation(ang + 90);
}

void Bullet::Draw(RenderWindow* window) {
    window->draw(rect);
    Character::Draw(window);
}

void Bullet::SetShapeColor(Color color) {
    Character::SetShapeColor(color);
    rect.setFillColor(color);
    color = rect.getFillColor();
    color.a = 50;
    rect.setOutlineColor(color);
}