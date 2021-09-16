#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>

#include "Utilities.h"

#define CHARCOLOR Color(200, 200, 200, 255)

using namespace std;
using namespace sf;

class Character {
	protected:
		float posX, posY;
		float size;
		float speed;

		float dirX, dirY;

		CircleShape shape;

	public:
		Character(float posX, float posY, float size, float speed);
		~Character();

		void Draw(RenderWindow* window);
		void Move(float deltaTime);
		bool Collision(Character* character);
		bool Collision(Character* character, float range);

		float GetPosX();
		float GetPosY();
		float GetSize();
		CircleShape* GetShape();

		void SetPos(float posX, float posY);
		void SetDir(float x, float y);
		void SetShapeColor(Color color);

		float GetDirX();
		float GetDirY();
};