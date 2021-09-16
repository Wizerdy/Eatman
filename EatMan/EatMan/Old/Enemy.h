#pragma once

#include "Character.h"
#include "Bullet.h"

#define ENEMYSIZE 20
#define ENEMYSPEED 100

#define ENEMYRANGEFACTOR 0.8f

enum class Body { LEGS, ARMS, BELLY, HEAD };

enum class Behaviour { COMING, MOTIONLESS, FLEEING };
enum class State { FEAR, SLOW };

class Enemy : public Character {
    private:
        int life;
        
        float shootRange;
        float reloadTime;
        float chronoShoot;

        float speedFactor;

        Character* target;

        Text lifeText;
        Font font;

    public:
        list<Bullet*> bullets;
        Behaviour behaviour;
        map<State, bool> states;
        bool canShoot;

        Enemy(float posX, float posY, float size, float speed, float reloadTime, float shootRange);
        ~Enemy();

        void Draw(RenderWindow* window);
        void Act(float deltaTime);
        void Move(float deltaTime, float speedFactor = 1);
        void MoveTo(Character* character);
        void FleeFrom(Character* character);
        void StopMove();
        void Shoot(float posX, float posY);
        void Shoot(float posX, float posY, float time);

        Body Wound();
        void Wounded(Body body);

        bool HasTarget();
        void SetTarget(Character* character);

        void SetLife(int life);
        void SetChronoShoot(float chrono);
        void SetShapeColor(Color color);
        int GetLife();
        float GetShootRange();
        float GetChronoShoot();
        float GetReloadTime();
        Character* GetTarget();

};