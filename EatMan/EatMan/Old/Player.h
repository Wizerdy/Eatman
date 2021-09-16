#pragma once

#include "Enemy.h"
#include "Bullet.h"

#define DYINGTIME 1.0f

class Player {
	private :
		Bullet* bullet;
		Enemy* infected;

		float damageTime;
		float deadTime;

	public :

		Player();
		~Player();
		void Shoot(float x, float y);
		void Infect(Enemy* enemy);

		bool Act(float time, bool* gameOver);
		void DamageInfected(int damage, float time);

		bool HasInfected();
		Enemy* GetInfected();
		Bullet* GetBullet();
};