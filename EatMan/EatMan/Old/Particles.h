#pragma once

#include "Character.h"

#define RADTODEG 57.2957795130823f

float Rand(float min, float max);
int RandInt(int min, int max);

class Particle : public Character {
	private:
		float lifeTime;
		float birthFrame;
		bool dead;

	public:

		Particle(float posX, float posY, float size, float speed, float lifeTime);
		~Particle();

		void Draw(RenderWindow* window);
		void Act(float elapsedTime, float deltaTime);
		void Move(float elapsedTime, float deltaTime);

		bool IsDead();
};

class GroupParticles {
	private:
		list<Particle*> particles;
		bool dead;

	public:
		GroupParticles(float number, float size, float speed, float lifeTime, float posX, float posY, float dirX, float dirY, float spray);
		GroupParticles(float number, float sizeMin, float sizeMax, float speedMin,
			float speedMax, float lifeTimeMin, float lifeTimeMax, float posX, float posY, float dirX, float dirY, float spray);
		~GroupParticles();

		void Draw(RenderWindow* window);
		void Move(float deltaTime);
		void Act(float elapsedTime, float deltaTime);

		bool IsDead();
};