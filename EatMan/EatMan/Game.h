#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <list>

#include "Utilities.h"
#include "Board.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Particles.h"
#include "InstantText.h"

#define RECOIL 5.0f

#define SCOREBUILDERMAX 1.0f
#define SCOREFACTORADD 0.1f

#define SCOREONKILL 100
#define SCOREONEAT 20
#define SCOREBUILDERONEAT 0.2f
#define SCOREBUILDERONFEAR 0.2f
#define SCOREBUILDERONKILL 0.6f


#define SPAWNTIME 0.5f
#define BULLETTIME 0.35f
#define RELOADTIME 1.0f

#define VOLUMEDIMINUTION 1.0f
#define SOUNDSMAX 50

using namespace std;

struct BulletShot {
	Bullet* bullet;
	Enemy* enemy;

	BulletShot(Bullet* bul, Enemy* ene) : bullet(bul), enemy(ene) {}
};

class Game {
	private :
		Board* board;
		list<Enemy*> enemies;
		list<BulletShot*> bullets;
		Player* player;
		RenderWindow* window;

		Font font;
		Text scoreText;
		Text scoreFactorText;
		int score;
		float scoreFactor;
		float scoreBuilder;
		
		bool gameOver;

		float elapsedTime;
		float deltaTime;
		float timeFactor;
		bool bulletTime;

		int difficult;
		float spawnTime;

		list<GroupParticles*> groupParticles;
		InstantTextList* shotTexts;

		SoundBuffer bufDeath;
		SoundBuffer bufShoot;

		SoundBuffer buffers[50];
		Sound sounds[50];
		Music music;
		float volume;

		void BuildScoreFactor(float add);
		void ResetScoreFactor();

	public :
		Game(RenderWindow* window);
		~Game();

		void Draw();
		void Act();
		void EnemyBehaviour(Enemy* enemy);
		void Collision();
		void BulletTime();
		void SpawnEnemies();

		void SpawnEnemy(float posX, float posY);
		void SpawnBleedParticles(Character* target, Character* object, float sizeFactor = 1);
		void SpawnBleedParticles(float posX, float posY, float dirX, float dirY, float sizeFactor = 1);
		void SpawnDeadParticles(float posX, float posY, Color color);

		void Recoil(Character* target, Character* object, float add = 0);
		void Shoot(float posX, float posY);
		void Infect(Enemy* enemy);
		void AddScore(int add);
		void Wave(Color color);
		void Impact(float posX, float posY, Color color);
		void Impact(float posX, float posY, float factor);

		void AddSound(SoundBuffer buf);
		void SetVolume(float vol);
		void SetDeltaTime(float deltaTime);
		void SetScoreText(int score);
		void SetBoard(Board* board);
		float GetElapsedTime();

		bool IsOutWindow(Character* character);
		bool GameOver();

};