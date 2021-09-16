#include "Game.h"

Game::Game(RenderWindow* window) {
	this->window = window;

	//board = new Board(window->getSize().x, window->getSize().y, 22, 44, Color(20, 20, 20, 255));
	board = new Board(window->getSize().x, window->getSize().y, 22, 10, Color(20, 20, 20, 255), true, 3);

	player = new Player();

	elapsedTime = 0;
	deltaTime = 0;
	timeFactor = 1;
	spawnTime = 0;
	gameOver = false;
	score = 0;

	font.loadFromFile(getAssetsPath() + "Assets/font.ttf");

	scoreText.setFont(font);
	scoreText.setCharacterSize(50);
	scoreText.setPosition(window->getSize().x * 1 / 30, window->getSize().y * 1 / 30);
	scoreText.setFillColor(CHARCOLOR);

	SetScoreText(score);

	SpawnEnemy(window->getSize().x / 2, window->getSize().y / 2);
	Infect((*enemies.begin()));

	//test = new GroupParticles(20, 1, 50, 1, window->getSize().x / 3, window->getSize().y / 3, -0.7f, -0.7f, 30.0f);
	//test = new GroupParticles(20, 0.5f, 2.5f, 40, 100, 0.6f, 0.6f, window->getSize().x / 3, window->getSize().y / 3, -0.7f, -0.7f, 45.0f);
}

Game::~Game() {
	delete board;
	delete player;
	for (Enemy* enemy : enemies) {
		delete enemy;
	}

	for (GroupParticles* particles : bleed) {
		delete particles;
	}
}

void Game::SpawnEnemy(float posX, float posY) {
	enemies.push_back(new Enemy(posX, posY, ENEMYSIZE, ENEMYSPEED, RELOADTIME, rand() % 100 + 200));
	if (player->HasInfected()) {
		enemies.back()->SetTarget(player->GetInfected());
	}
}

void Game::Draw() {
	board->Draw(window);

	for (Enemy* enemy : enemies) {
		enemy->Draw(window);
		for (Bullet* bullet : enemy->bullets) {
			if (bullet->isActive) {
				bullet->Draw(window);
			}
		}
	}

	if (player->GetBullet()->isActive) {
		player->GetBullet()->Draw(window);
	}

	for (GroupParticles* particles : bleed)
	{
		particles->Draw(window);
	}
	window->draw(scoreText);
}

void Game::Act() {
	board->Act(elapsedTime);

	//test->Act(elapsedTime, deltaTime);
	for (list<GroupParticles*>::iterator particles = bleed.begin(); particles != bleed.end(); particles++)
	{
		(*particles)->Act(elapsedTime, deltaTime);
		if ((*particles)->IsDead()) {
			delete (*particles);
			particles = bleed.erase(particles);

			if (particles == bleed.end()) {
				break;
			}
		}
	}

	if (player->Act(elapsedTime, &gameOver)) {
		AddScore(10);
	}

	// Boucle des enemies
	for (list<Enemy*>::iterator enemy = enemies.begin(); enemy != enemies.end(); enemy++) {

		// Destruction de l'enemie s'il fuit loin ou si il a plus de vie
		if (((*enemy)->behaviour == Behaviour::FLEEING && IsOutWindow(*enemy) && (*enemy)->states[State::FEAR] == true) ||
			((*enemy)->GetLife() <= 0 && ((*enemy) != player->GetInfected() || player->GetBullet()->isActive))
		) {

			if ((*enemy)->GetLife() <= 0 && ((*enemy) != player->GetInfected() || player->GetBullet()->isActive)) {
				AddScore(SCOREONKILL);
			}

			delete (*enemy);
			enemy = enemies.erase(enemy);
			if (enemy == enemies.end()) {
				break;
			}
		}

		if ((*enemy)->HasTarget() && (!player->HasInfected() || (*enemy) != player->GetInfected())) {
			(*enemy)->Act(deltaTime);

			#pragma region Old version
			// Comportement des enemies
			//if ((*enemy)->Collision(player->GetInfected(), (*enemy)->GetShootRange())) {
			//	(*enemy)->behaviour = Behaviour::MOTIONLESS;

			//	// Fuit
			//	if ((*enemy)->Collision(player->GetInfected(), (*enemy)->GetShootRange() / 1.2f)) {
			//		(*enemy)->FleeFrom(player->GetInfected()->GetPosX(), player->GetInfected()->GetPosY());
			//		(*enemy)->behaviour = Behaviour::FLEEING;
			//		(*enemy)->SetShapeColor(CHARCOLOR);
			//		(*enemy)->canShoot = false;
			//	}
			//	// Tir
			//	else if ((*enemy)->canShoot && elapsedTime - (*enemy)->GetChronoShoot() > (*enemy)->GetReloadTime()) {
			//		(*enemy)->Shoot(player->GetInfected()->GetPosX(), player->GetInfected()->GetPosY(), elapsedTime);
			//		(*enemy)->SetShapeColor(CHARCOLOR);
			//	}
			//	// Se prepare a tirer
			//	else if ((*enemy)->canShoot) {
			//		(*enemy)->SetShapeColor(Color(
			//			CHARCOLOR.r,
			//			CHARCOLOR.g - ((elapsedTime - (*enemy)->GetChronoShoot()) / (*enemy)->GetReloadTime()) * CHARCOLOR.g,
			//			CHARCOLOR.b - ((elapsedTime - (*enemy)->GetChronoShoot()) / (*enemy)->GetReloadTime()) * CHARCOLOR.b,
			//			CHARCOLOR.a
			//		));
			//	}
			//	// Initialise le tir
			//	else {
			//		(*enemy)->canShoot = true;
			//		(*enemy)->SetChronoShoot(elapsedTime);
			//	}
			//}
			//// S'avance vers le player
			//else {
			//	(*enemy)->behaviour = Behaviour::COMING;
			//	(*enemy)->MoveTo(player->GetInfected()->GetPosX(), player->GetInfected()->GetPosY());
			//	(*enemy)->canShoot = false;
			//	(*enemy)->SetShapeColor(CHARCOLOR);
			//}
			#pragma endregion

			switch ((*enemy)->behaviour)
			{
				case Behaviour::COMING: // Avancer
					if ((*enemy)->Collision((*enemy)->GetTarget(), (*enemy)->GetShootRange())) {
						(*enemy)->StopMove();
					}
					break;
				case Behaviour::MOTIONLESS: // Tirer si a portée
					if ((*enemy)->Collision((*enemy)->GetTarget(), (*enemy)->GetShootRange())) {
						// Tirer
						if ((*enemy)->canShoot && elapsedTime - (*enemy)->GetChronoShoot() > (*enemy)->GetReloadTime()) {
							(*enemy)->Shoot((*enemy)->GetTarget()->GetPosX(), (*enemy)->GetTarget()->GetPosY(), elapsedTime);
							(*enemy)->SetShapeColor(CHARCOLOR);
						}
						// Se prepare a tirer
						else if ((*enemy)->canShoot) {
							(*enemy)->SetShapeColor(Color(
								CHARCOLOR.r,
								CHARCOLOR.g - ((elapsedTime - (*enemy)->GetChronoShoot()) / (*enemy)->GetReloadTime()) * CHARCOLOR.g,
								CHARCOLOR.b - ((elapsedTime - (*enemy)->GetChronoShoot()) / (*enemy)->GetReloadTime()) * CHARCOLOR.b,
								CHARCOLOR.a
							));
						}
						// Initialise le tir
						else {
							(*enemy)->canShoot = true;
							(*enemy)->SetChronoShoot(elapsedTime);
						}
					}
					break;
				case Behaviour::FLEEING:
					if (!(*enemy)->states[State::FEAR] && !(*enemy)->Collision((*enemy)->GetTarget(), (*enemy)->GetShootRange() / 1.2f)) {
						(*enemy)->StopMove();
					}
					break;
			}
		}

		// Boucle des bullets
		for (Bullet* bullet : (*enemy)->bullets) {
			if (bullet->isActive) {
				bullet->Move(deltaTime);
			}
		}
		if (enemy == enemies.end()) {
			break;
		}
	}

	if (player->GetBullet()->isActive) {
		player->GetBullet()->Move(deltaTime);
	}

	Collision();
	BulletTime();
	SpawnEnemies();

	if (gameOver) {
		timeFactor = 0;
	}
}


void Game::Collision() {
	// Bullet player en dehors de l'ecran
	if (IsOutWindow(player->GetBullet())) {
		player->GetBullet()->isActive = false;
		bulletTime = false;
		gameOver = true;
	}

	for(list<Enemy*>::iterator enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
		// Collision bullet player
		if (player->GetBullet()->isActive && (*enemy) != player->GetInfected() && player->GetBullet()->Collision((*enemy))) {
			player->Infect((*enemy));
			player->GetInfected()->StopMove();

			Recoil((*enemy), player->GetBullet(), RECOIL);
			Body wound = (*enemy)->Wound();
			(*enemy)->Wounded(wound);
			SpawnParticles((*enemy), player->GetBullet(), ParticleSize((int)wound));


			player->GetBullet()->isActive = false;
			bulletTime = false;

			(*enemy)->SetShapeColor(Color(0, CHARCOLOR.g, 0, CHARCOLOR.a));

			player->DamageInfected(0, elapsedTime);

			for (Enemy* other : enemies) {
				if (other != player->GetInfected()) {
					other->SetTarget(player->GetInfected());
				}
			}
		}

		// Collision entre enemies
		if ((*enemy) != player->GetInfected()) {
			for (Enemy* other : enemies) {
				if ((*enemy) != other && (*enemy)->Collision(other)) {
					Recoil((*enemy), other);
				}
			}
		}

		// Collision entre les bullets puis enemies
		for (list<Bullet*>::iterator bullet = (*enemy)->bullets.begin(); bullet != (*enemy)->bullets.end(); bullet++) {
			if ((*bullet)->isActive) {

				if (player->HasInfected() && (*bullet)->Collision(player->GetInfected())) {
					Body wound = (*enemy)->Wound();
					player->GetInfected()->Wounded(wound);
					SpawnParticles(player->GetInfected(), (*bullet), ParticleSize((int)wound));

					if (player->GetBullet()->isActive) {
						if (player->GetInfected()->GetLife() <= 0) {
							delete player->GetInfected();
							enemies.remove(player->GetInfected());
							player->Infect(nullptr);

							AddScore(SCOREONKILL);
						}
						else {
							Recoil(player->GetInfected(), (*bullet), RECOIL);
						}
					}
					else {
						Recoil(player->GetInfected(), (*bullet), RECOIL);
						player->DamageInfected(0, elapsedTime);
					}
					delete (*bullet);
					bullet = (*enemy)->bullets.erase(bullet);
				}
				else if (player->GetBullet()->isActive && (*bullet)->Collision(player->GetBullet())) {
					gameOver = true;
				}
				else if (IsOutWindow((*bullet))) {
					delete (*bullet);
					bullet = (*enemy)->bullets.erase(bullet);
				}
				else {
					for (list<Enemy*>::iterator other = enemies.begin(); other != enemies.end(); other++) {
						if ((*other) != (*enemy) && (*bullet)->Collision((*other))) {

							// Damage
							Body wound = (*other)->Wound();
							(*other)->Wounded(wound);
							SpawnParticles((*other), (*bullet), ParticleSize((int)wound));

							if ((*other)->GetLife() <= 0) {
								delete (*other);
								other = enemies.erase(other);

								AddScore(SCOREONKILL);
							}
							else {
								// Shake
								Recoil((*other), (*bullet), RECOIL);
							}
							delete (*bullet);
							bullet = (*enemy)->bullets.erase(bullet);
							break;
						}
					}
				}
			}
			if (bullet == (*enemy)->bullets.end()) {
				break;
			}

		}
		if (enemy == enemies.end()) {
			break;
		}
	}
}

void Game::BulletTime() {
	if (bulletTime) {
		if (timeFactor > BULLETTIME) {
			//timeFactor -= 10 * deltaTime;
			float time = 1.0f / (((1.0f - timeFactor) + 0.1f) * 0.6f) * deltaTime;
			timeFactor -= time;
		}
	}
	else {
		if (timeFactor < 1) {
			//timeFactor += 10 * deltaTime;
			float time = 1.0f / (((1.0f - timeFactor) + 0.1f) * 0.6f) * deltaTime;
			timeFactor += time;
		}
	}

	if (timeFactor > 1) {
		timeFactor = 1;
	}
}

void Game::SpawnEnemies() {
	if (elapsedTime - spawnTime > SPAWNTIME) {
		if (rand() % 2 == 0) { // DROITE GAUCHE
			SpawnEnemy((window->getSize().x + ENEMYSIZE * 1.2f * 2) * (rand() % 2) - ENEMYSIZE * 1.2f, rand() % window->getSize().y);
		}
		else { // HAUT BAS
			SpawnEnemy(rand() % window->getSize().x, (window->getSize().y + ENEMYSIZE * 2 * 1.2f) * (rand() % 2) - ENEMYSIZE * 1.2f);
		}

		spawnTime = elapsedTime;
	}
}

void Game::SpawnParticles(float posX, float posY, float dirX, float dirY, float sizeFactor) {
	//test = new GroupParticles(20, 0.5f, 2.5f, 40, 100, 0.6f, 0.6f, window->getSize().x / 3, window->getSize().y / 3, -0.7f, -0.7f, 45.0f);
	//bleed.push_back(new GroupParticles(20, 0.5f, 2.5f, 40, 100, 0.6f, 0.6f, posX, posY, dirX, dirY, 45.0f));
	/*(float number, float sizeMin, float sizeMax, float speedMin, float speedMax, float lifeTimeMin, 
		float lifeTimeMax, float posX, float posY, float dirX, float dirY, float spray);*/
	//cout << sizeFactor << endl;
	bleed.push_back(new GroupParticles(5, (float)ENEMYSIZE / 6.5f * sizeFactor, (float)ENEMYSIZE / 6.0f * sizeFactor, 70, 150, 0.2f, 0.2f, posX, posY, dirX, dirY, 90.0f));
}

void Game::SpawnParticles(Character* target, Character* object, float sizeFactor) {
	float x = object->GetPosX() - target->GetPosX();
	float y = object->GetPosY() - target->GetPosY();
	float mag = sqrt(x * x + y * y);

	SpawnParticles(
		target->GetPosX() + (x * (ENEMYSIZE + BULLETSIZE) / mag),
		target->GetPosY() + (y * (ENEMYSIZE + BULLETSIZE) / mag),
		x / mag,
		y / mag,
		sizeFactor
	);
}

void Game::Infect(Enemy* enemy) {
	player->Infect(enemy);
}

void Game::Shoot(float posX, float posY) {
	if (!player->GetBullet()->isActive) {
		player->Shoot(posX, posY);
		player->GetInfected()->SetShapeColor(CHARCOLOR);
		bulletTime = true;
	}
}

void Game::SetDeltaTime(float deltaTime) {
	this->elapsedTime += deltaTime * timeFactor;
	this->deltaTime = deltaTime * timeFactor;
}

bool Game::IsOutWindow(Character* character) {
	if (character->GetPosX() + character->GetSize() < 0 ||
		character->GetPosX() - character->GetSize() > window->getSize().x ||
		character->GetPosY() + character->GetSize() < 0 ||
		character->GetPosY() - character->GetSize() > window->getSize().y)
	{
		return true;
	}
	return false;
}

float Game::GetElapsedTime() {
	return elapsedTime;
}

void Game::Recoil(Character* target, Character* object, float add) {
	float x = target->GetPosX() - object->GetPosX();
	float y = target->GetPosY() - object->GetPosY();
	float mag = sqrt(x * x + y * y);

	if (mag == 0) {
		cout << mag << " : " << x << " + " << y << " // " <<
			target->GetPosX() << " + " << target->GetPosY() << " . " <<
			object->GetPosX() << " + " << object->GetPosY() << endl;
	}

	target->SetPos(
		target->GetPosX() + (x * (target->GetSize() + object->GetSize() + add) / mag) - x,
		target->GetPosY() + (y * (target->GetSize() + object->GetSize() + add) / mag) - y
	);
}

void Game::AddScore(int add) {
	score += add;
	float delta = 40 * (add + 40) / (SCOREONKILL + 40);
	board->Wave(Color(board->GetColor().r + delta, board->GetColor().g + delta, board->GetColor().b + delta, 255));
	SetScoreText(score);
}

void Game::SetScoreText(int score) {
	scoreText.setString(std::to_string(score));
}

float ParticleSize(int size) {
	switch (size) {
		case 0:
			return 1.0f;
		case 1:
			return 1.0f;
		case 2:
			return 1.5f;
		case 3:
			return 2.0f;
		default:
			return 2.5f;
	}
}

void Game::Wave(Color color) {
	board->Wave(color);
}