#pragma once

#include "Character.h"

#define RADTODEG 57.2957795130823f

#define BULLETSIZE 3
#define BULLETSPEED 500

class Bullet : public Character {
    private:
        RectangleShape rect;

    public:
        bool isActive;

        Bullet(float posX, float posY, float size, float speed);
        ~Bullet();

        void Draw(RenderWindow* window);
        void Move(float deltaTime);

        void SetPos(float posX, float posY);
        void SetDir(float x, float y);
        void SetShapeColor(Color color);

};