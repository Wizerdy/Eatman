#include "Character.h"

Character::Character(float posX, float posY, float size, float speed) {
	this->posX = posX;
	this->posY = posY;
	this->size = size;
	this->speed = speed;

	//shape.setFillColor(Color(200, 50, 50, 200));
	shape.setOrigin(size, size);
	shape.setPosition(posX, posY);
	shape.setRadius(size);

	shape.setOutlineThickness(4);

	SetShapeColor(CHARCOLOR);

	dirX = 0;
	dirY = 0;
}

Character::~Character() {
};

void Character::Draw(RenderWindow* window) {
	window->draw(shape);
}

CircleShape* Character::GetShape() {
	return &shape;
}

void Character::SetShapeColor(Color color) {
	shape.setFillColor(color);
	color = shape.getFillColor();
	color.a /= 4;
	shape.setOutlineColor(color);
}

void Character::Move(float deltaTime) {
	SetPos(posX + dirX * speed * deltaTime, posY + dirY * speed * deltaTime);
}

bool Character::Collision(Character* character) {
	float x = posX - character->posX;
	float y = posY - character->posY;
	if (size + character->size + 2 > sqrt(x * x + y * y)) {
		return true;
	}
	return false;
}

bool Character::Collision(Character* character, float range) {
	float x = posX - character->posX;
	float y = posY - character->posY;
	if (range > sqrt(x * x + y * y)) {
		return true;
	}
	return false;
}


float Character::GetPosX() {
	return posX;
}

float Character::GetPosY() {
	return posY;
}

float Character::GetSize() {
	return size;
}

void Character::SetPos(float posX, float posY) {
	this->posX = posX;
	this->posY = posY;

	shape.setPosition(posX, posY);
}

void Character::SetDir(float x, float y) {
	dirX = x;
	dirY = y;
}

float Character::GetDirX() {
	return dirX;
}

float Character::GetDirY() {
	return dirY;
}