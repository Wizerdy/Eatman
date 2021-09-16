#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>

#include "Utilities.h"
#include "Board.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Particles.h"

#define RECOIL 5.0f

#define SCOREONKILL 100
#define SPAWNTIME 0.5f
#define BULLETTIME 0.45f
#define RELOADTIME 1.0f

using namespace std;

float ParticleSize(int size);

class Game {
	private :
		Board* board;
		list<Enemy*> enemies;
		Player* player;
		RenderWindow* window;

		Font font;
		Text scoreText;
		int score;
		
		bool gameOver;

		float elapsedTime;
		float deltaTime;
		float timeFactor;
		bool bulletTime;

		int difficult;
		float spawnTime;

		list<GroupParticles*> bleed;

	public :
		Game(RenderWindow* window);
		~Game();
		void SpawnEnemy(float posX, float posY);

		void Draw();
		void Act();
		void Collision();
		void BulletTime();
		void SpawnEnemies();

		void SpawnParticles(float posX, float posY, float dirX, float dirY, float sizeFactor = 1);
		void SpawnParticles(Character* target, Character* object, float sizeFactor = 1);

		void Infect(Enemy* enemy);
		void Shoot(float posX, float posY);
		void SetDeltaTime(float deltaTime);
		void Wave(Color color);
		float GetElapsedTime();

		bool IsOutWindow(Character* character);

		void AddScore(int add);
		void SetScoreText(int score);

		void Recoil(Character* target, Character* object, float add = 0);
};