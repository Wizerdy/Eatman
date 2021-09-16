#include "Game.h"

Game::Game(RenderWindow* window) {
	this->window = window;

	//board = new Board(window->getSize().x, window->getSize().y, 22, 44, Color(20, 20, 20, 255));
	board = new Board(window->getSize().x, window->getSize().y, 22, 10, Color(20, 20, 20, 255), true, 3);

	//board = new Board(window->getSize().x, window->getSize().y, 22, 10, Color(10, 10, 10, 255));
	//board = new Board(window->getSize().x, window->getSize().y, 22, -11, Color(20, 20, 20, 255), true, 3);
	//board = new Board(window->getSize().x, window->getSize().y, 22, 44, Color(80, 40, 0, 255));

	player = new Player();

	elapsedTime = 0;
	deltaTime = 0;
	timeFactor = 1;
	spawnTime = 0;
	gameOver = false;
	score = 0;
	scoreFactor = 1;

	bufDeath.loadFromFile(getAssetsPath() + "Assets/Death.wav");
	bufShoot.loadFromFile(getAssetsPath() + "Assets/Shoot.wav");

	music.openFromFile(getAssetsPath() + "Assets/Music.wav");
	music.setLoopPoints(Music::TimeSpan(seconds(40), seconds(23)));
	music.setLoop(true);

	volume = 25.0f;
	music.setVolume(volume);
	music.play();

	for (int i = 0; i < 50; i++)
	{
		sounds[i].setVolume(volume/1.5f);
	}

	font.loadFromFile(getAssetsPath() + "Assets/font.ttf");

	scoreText.setFont(font);
	scoreText.setCharacterSize(60);
	scoreText.setPosition(window->getSize().x * 1 / 30, window->getSize().y * 1 / 30);
	scoreText.setFillColor(CHARCOLOR);

	SetScoreText(score);

	scoreFactorText.setFont(font);
	scoreFactorText.setCharacterSize(40);
	scoreFactorText.setOrigin(0, scoreFactorText.getCharacterSize() / 2);
	scoreFactorText.setFillColor(CHARCOLOR);

	scoreFactorText.setString(to_string(scoreFactor));

	SpawnEnemy(window->getSize().x / 2, window->getSize().y / 2);
	Infect((*enemies.begin()));
	(*enemies.begin())->SetShapeColor(Color(0, CHARCOLOR.g, 0, CHARCOLOR.a));
	(*enemies.begin())->SetLife(7);
	(*enemies.begin())->states[State::FEAR] = true;

	shotTexts = new InstantTextList(window->getSize().x * 1 / 30, window->getSize().y * 1 / 30 + 60, font, 30, CHARCOLOR);
}

Game::~Game() {
	//delete board;
	delete player;
	for (Enemy* enemy : enemies) {
		delete enemy;
	}

	for (BulletShot* bullet : bullets) {
		delete bullet->bullet;
	}

	for (GroupParticles* particles : groupParticles) {
		delete particles;
	}

	delete shotTexts;
}


void Game::Draw() {
	board->Draw(window);

	for (Enemy* enemy : enemies) {
		enemy->Draw(window);
	}

	for (BulletShot* bullet : bullets) {
		if (bullet->bullet->isActive) {
			bullet->bullet->Draw(window);
		}
	}

	if (player->GetBullet()->isActive) {
		player->GetBullet()->Draw(window);
	}

	for (GroupParticles* particles : groupParticles)
	{
		particles->Draw(window);
	}

	shotTexts->Draw(window);
	window->draw(scoreText);
	if (scoreFactor > 1) {
		window->draw(scoreFactorText);
	}
}

void Game::Act() {

	board->Act(elapsedTime);

	//test->Act(elapsedTime, deltaTime);
	for (list<GroupParticles*>::iterator particles = groupParticles.begin(); particles != groupParticles.end(); particles++)
	{
		(*particles)->Act(elapsedTime, deltaTime);
		if ((*particles)->IsDead()) {
			delete (*particles);
			particles = groupParticles.erase(particles);

			if (particles == groupParticles.end()) {
				break;
			}
		}
	}

	shotTexts->Act(elapsedTime, deltaTime);

	if (player->Act(elapsedTime, &gameOver)) {
		AddScore(SCOREONEAT);
		BuildScoreFactor(SCOREBUILDERONEAT);
	}


	// Boucle des enemies
	for (list<Enemy*>::iterator enemy = enemies.begin(); enemy != enemies.end(); enemy++) {

		// Destruction de l'enemie s'il fuit loin ou si il a plus de vie
		if (((*enemy)->behaviour == Behaviour::FLEEING && IsOutWindow(*enemy) && (*enemy)->states[State::FEAR] == true) ||
			((*enemy)->GetLife() <= 0 && (!player->IsInfected(*enemy) || !player->IsInsideInfected())))
		{
			if ((*enemy)->GetLife() <= 0 && (!player->IsInfected(*enemy) || !player->IsInsideInfected())) {
				if ((*enemy)->states[State::FEAR]) {
					BuildScoreFactor(SCOREBUILDERONFEAR);
				}
				BuildScoreFactor(SCOREBUILDERONKILL);
				AddScore(SCOREONKILL);
				AddSound(bufDeath);
				SpawnDeadParticles((*enemy)->GetPosX(), (*enemy)->GetPosY(), (*enemy)->GetShape()->getFillColor());
			}
			else if((*enemy)->states[State::FEAR]) {
				BuildScoreFactor(-SCOREBUILDERONFEAR);
			}
			delete (*enemy);
			enemy = enemies.erase(enemy);
			if (enemy == enemies.end()) {
				break;
			}
		}

		if ((*enemy)->HasTarget() && !player->IsInfected(*enemy)) {
			(*enemy)->Act(deltaTime);

			EnemyBehaviour(*enemy);
		}
		else if (!player->IsInfected(*enemy) && (*enemy)->behaviour == Behaviour::MOTIONLESS) {
			(*enemy)->SetShapeColor(CHARCOLOR);
		}

		if (enemy == enemies.end()) {
			break;
		}
	}

	// Boucle des bullets
	for (BulletShot* bullet : bullets) {
		if (bullet->bullet->isActive) {
			bullet->bullet->Move(deltaTime);
		}
	}

	if (player->GetBullet()->isActive) {
		player->GetBullet()->Move(deltaTime);
	}

	Collision();
	BulletTime();
	SpawnEnemies();

	if (gameOver && player->IsInsideInfected()) {
		SpawnDeadParticles(player->GetInfected()->GetPosX(), player->GetInfected()->GetPosY(), player->GetInfected()->GetShape()->getFillColor());
		enemies.remove(player->GetInfected());
		player->DeleteInfected();
	}

	if (gameOver) {
		bulletTime = true;
	}
}

void Game::EnemyBehaviour(Enemy* enemy) {
	switch (enemy->behaviour)
	{
	case Behaviour::COMING: // Avancer
		if (enemy->Collision(enemy->GetTarget(), enemy->GetShootRange())) {
			enemy->StopMove();
		}
		break;
	case Behaviour::MOTIONLESS: // Tirer si a portée
		if (enemy->Collision(enemy->GetTarget(), enemy->GetShootRange())) {
			// Tirer
			if (enemy->canShoot && elapsedTime - enemy->GetChronoShoot() > enemy->GetReloadTime()) {
				bullets.push_back(new BulletShot(enemy->Shoot(enemy->GetTarget()->GetPosX(), enemy->GetTarget()->GetPosY(), elapsedTime), enemy));
				enemy->SetShapeColor(CHARCOLOR);
				AddSound(bufShoot);
			}
			// Se prepare a tirer
			else if (enemy->canShoot) {
				enemy->SetShapeColor(Color(
					CHARCOLOR.r,
					CHARCOLOR.g - ((elapsedTime - enemy->GetChronoShoot()) / enemy->GetReloadTime()) * CHARCOLOR.g,
					CHARCOLOR.b - ((elapsedTime - enemy->GetChronoShoot()) / enemy->GetReloadTime()) * CHARCOLOR.b,
					CHARCOLOR.a
				));
			}
			// Initialise le tir
			else {
				enemy->canShoot = true;
				enemy->SetChronoShoot(elapsedTime);
			}
		}
		break;
	case Behaviour::FLEEING:
		if (!enemy->states[State::FEAR] && !enemy->Collision(enemy->GetTarget(), enemy->GetShootRange() / 1.2f)) {
			enemy->StopMove();
		}
		break;
	}
}

void Game::Collision() {
	// Bullet player en dehors de l'ecran
	if (!gameOver && IsOutWindow(player->GetBullet())) {
		//player->GetBullet()->isActive = false;
		SpawnDeadParticles(player->GetBullet()->GetPosX(), player->GetBullet()->GetPosY(), player->GetBullet()->GetShape()->getFillColor());
		gameOver = true;
	}

	for (list<Enemy*>::iterator enemy = enemies.begin(); enemy != enemies.end(); enemy++) {
		// Collision bullet player
		if (!player->IsInsideInfected() && !player->IsInfected(*enemy) && player->GetBullet()->Collision(*enemy)) {
			player->Infect((*enemy));
			player->GetInfected()->StopMove();

			Recoil((*enemy), player->GetBullet(), RECOIL);
			Body wound = (*enemy)->Wound();
			(*enemy)->Wounded(wound);
			SpawnBleedParticles((*enemy), player->GetBullet(), ParticleSize((int)wound));
			shotTexts->AddShot(elapsedTime, (int)wound);
			Impact(player->GetInfected()->GetPosX(), player->GetInfected()->GetPosY(), 1);

			player->GetBullet()->isActive = false;
			bulletTime = false;

			(*enemy)->SetShapeColor(Color(0, CHARCOLOR.g, 0, CHARCOLOR.a));

			player->DamageInfected(0, elapsedTime);

			for (Enemy* other : enemies) {
				if (!player->IsInfected(other)) {
					other->SetTarget(player->GetInfected());
				}
			}
		}

		// Collision entre enemies
		if (!player->IsInfected(*enemy)) {
			for (list<Enemy*>::iterator other = next(enemy); other != enemies.end(); other++) {
				if ((*enemy) != (*other) && (*enemy)->Collision((*other))) {
					Recoil((*enemy), (*other));
				}
			}
		}
	}

	// Collision entre les bullets puis enemies
	for (list<BulletShot*>::iterator bullet = bullets.begin(); bullet != bullets.end(); bullet++) {
		if ((*bullet)->bullet->isActive) {
			if (player->HasInfected() && (*bullet)->bullet->Collision(player->GetInfected())) {
				Body wound = player->GetInfected()->Wound();
				player->GetInfected()->Wounded(wound);
				SpawnBleedParticles(player->GetInfected(), (*bullet)->bullet, ParticleSize((int)wound));
				shotTexts->AddShot(elapsedTime, (int)wound);

				Impact(player->GetInfected()->GetPosX(), player->GetInfected()->GetPosY(), 1);

				if (!player->IsInsideInfected()) {
					if (player->GetInfected()->GetLife() <= 0) {
						if (player->GetInfected()->states[State::FEAR]) {
							BuildScoreFactor(SCOREBUILDERONFEAR);
						}
						BuildScoreFactor(SCOREBUILDERONKILL);
						AddScore(SCOREONKILL);
						SpawnDeadParticles(player->GetInfected()->GetPosX(), player->GetInfected()->GetPosY(), player->GetInfected()->GetShape()->getFillColor());

						enemies.remove(player->GetInfected());
						player->DeleteInfected();
						AddSound(bufDeath);
					}
					else {
						Recoil(player->GetInfected(), (*bullet)->bullet, RECOIL);
					}
				}
				else {
					Recoil(player->GetInfected(), (*bullet)->bullet, RECOIL);
					player->DamageInfected(0, elapsedTime);
					ResetScoreFactor();
				}
				delete (*bullet);
				bullet = bullets.erase(bullet);
			}
			else if (!player->IsInsideInfected() && (*bullet)->bullet->Collision(player->GetBullet())) {
				gameOver = true;
				SpawnDeadParticles(player->GetBullet()->GetPosX(), player->GetBullet()->GetPosY(), player->GetBullet()->GetShape()->getFillColor());
				player->GetBullet()->isActive = false;
				delete (*bullet);
				bullet = bullets.erase(bullet);
			}
			else if (IsOutWindow((*bullet)->bullet)) {
				delete (*bullet);
				bullet = bullets.erase(bullet);
			}
			else {
				for (list<Enemy*>::iterator other = enemies.begin(); other != enemies.end(); other++) {
					if ((*bullet)->enemy != (*other) && (*bullet)->bullet->Collision((*other))) {

						// Damage
						Body wound = (*other)->Wound();
						(*other)->Wounded(wound);
						SpawnBleedParticles((*other), (*bullet)->bullet, ParticleSize((int)wound));
						shotTexts->AddShot(elapsedTime, (int)wound);

						if ((*other)->GetLife() <= 0) {
							if ((*other)->states[State::FEAR]) {
								BuildScoreFactor(SCOREBUILDERONFEAR);
							}
							BuildScoreFactor(SCOREBUILDERONKILL);
							AddScore(SCOREONKILL);

							SpawnDeadParticles((*other)->GetPosX(), (*other)->GetPosY(), (*other)->GetShape()->getFillColor());
							delete (*other);
							other = enemies.erase(other);
							AddSound(bufDeath);
						}
						else {
							// Shake
							Recoil((*other), (*bullet)->bullet, RECOIL);
						}
						delete (*bullet);
						bullet = bullets.erase(bullet);
						break;
					}
				}
			}
		}

		if (bullet == bullets.end()) {
			break;
		}

	}
}

void Game::BulletTime() {
	if (bulletTime) {
		if (timeFactor > BULLETTIME || (gameOver && timeFactor > 0)) {
			float time = 1.0f / (((1.0f - timeFactor) + 0.25f) * 1.0f) * deltaTime;
			timeFactor -= time;
			//if (!gameOver) {
			//	music.setPitch(0.8f + timeFactor / 5);
			//}
			//else {
			//	music.setPitch(0.8f * timeFactor / 1.0f);
			//}
			//music.setVolume(volume - volume * VOLUMEDIMINUTION / timeFactor);
			if (gameOver) {
				if (volume - volume * VOLUMEDIMINUTION / (timeFactor * 10) < 1) {
					music.setVolume(0.0f);
				}
				else {
					music.setVolume(volume - volume * VOLUMEDIMINUTION / (timeFactor * 10));
				}
			}
		}
	}
	else {
		if (timeFactor < 1) {
			float time = 1.0f / (((1.0f - timeFactor) + 0.25f) * 0.6f) * deltaTime;
			timeFactor += time;
			//music.setPitch(0.8f + timeFactor / 5);
			//music.setVolume(volume - volume * VOLUMEDIMINUTION / timeFactor);
		}
	}

	if (timeFactor > 1) {
		timeFactor = 1;
		//music.setPitch(1.0f);
	}
	else if (timeFactor < 0.1f) {
		timeFactor = 0.0f;
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

void Game::Recoil(Character* target, Character* object, float add) {
	float x = target->GetPosX() - object->GetPosX();
	float y = target->GetPosY() - object->GetPosY();
	float mag = sqrt(x * x + y * y);

	if (mag == 0) {
		/*cout << mag << " : " << x << " + " << y << " // " <<
			target->GetPosX() << " + " << target->GetPosY() << " . " <<
			object->GetPosX() << " + " << object->GetPosY() << endl;*/
	}

	target->SetPos(
		target->GetPosX() + (x * (target->GetSize() + object->GetSize() + add) / mag) - x,
		target->GetPosY() + (y * (target->GetSize() + object->GetSize() + add) / mag) - y
	);
}

void Game::SpawnEnemy(float posX, float posY) {
	enemies.push_back(new Enemy(posX, posY, ENEMYSIZE, ENEMYSPEED, RELOADTIME, rand() % 100 + 200));
	if (player->HasInfected()) {
		enemies.back()->SetTarget(player->GetInfected());
	}
}

void Game::SpawnBleedParticles(Character* target, Character* object, float sizeFactor) {
	float x = object->GetPosX() - target->GetPosX();
	float y = object->GetPosY() - target->GetPosY();
	float mag = sqrt(x * x + y * y);

	SpawnBleedParticles(
		target->GetPosX() + (x * (ENEMYSIZE + BULLETSIZE) / mag),
		target->GetPosY() + (y * (ENEMYSIZE + BULLETSIZE) / mag),
		x / mag,
		y / mag,
		sizeFactor
	);
}

void Game::SpawnBleedParticles(float posX, float posY, float dirX, float dirY, float sizeFactor) {
	groupParticles.push_back(new GroupParticles(
		int(4 * sizeFactor), (float)ENEMYSIZE / 10.0f * sizeFactor, (float)ENEMYSIZE / 6.0f * sizeFactor,
		70, 150, 0.2f, 0.2f, posX, posY, dirX, dirY, 90.0f, Color(230, 10, 10, 255)
	));
}

void Game::SpawnDeadParticles(float posX, float posY, Color color) {
	groupParticles.push_back(new GroupParticles(
		5, 5, 65, 0.3f, posX, posY, 1, 0, 360, color
	));
	/*groupParticles.push_back(new GroupParticles(
		13, 4, 50, 0.3f, posX, posY, 1, 0, 360, color
	));*/
}


void Game::Infect(Enemy* enemy) {
	player->Infect(enemy);
}

void Game::AddScore(int add) {
	score += add * scoreFactor;
	float delta = 50 * (add * scoreFactor + 40) / (SCOREONKILL * scoreFactor + 40);
	board->Wave(Color(board->GetColor().r + delta, board->GetColor().g + delta, board->GetColor().b + delta, 255));
	SetScoreText(score);

	scoreFactorText.setPosition(
		scoreText.getLocalBounds().width + scoreText.getPosition().x + 10,
		scoreText.getLocalBounds().height / 2 + scoreText.getPosition().y
	);
}

void Game::BuildScoreFactor(float add) {
	scoreBuilder += add;
	if (scoreBuilder >= SCOREBUILDERMAX) {
		scoreFactor += SCOREFACTORADD;
		scoreBuilder -= SCOREBUILDERMAX;
		scoreFactorText.setString("x " + to_string(scoreFactor).substr(0, 3));
	}
}

void Game::ResetScoreFactor() {
	scoreBuilder = 0.0f;
	scoreFactor = 1.0f;
}

void Game::Shoot(float posX, float posY) {
	if (!gameOver && !player->GetBullet()->isActive) {
		player->Shoot(posX, posY);
		AddSound(bufShoot);
		if (player->GetInfected()->GetLife() > 0) {
			player->GetInfected()->SetShapeColor(CHARCOLOR);
		}
		bulletTime = true;
	}
}

void Game::Wave(Color color) {
	board->Wave(color);
}

void Game::Impact(float posX, float posY, Color color) {
	board->Impact(posX, posY, color);
}

void Game::Impact(float posX, float posY, float intensity) {
	float delta = 70 * (intensity * 10) / 10;
	Color color = Color(board->GetColor().r + delta, board->GetColor().g + delta, board->GetColor().b + delta, board->GetColor().a);
	//board->Impact(posX, posY, color);
}

void Game::AddSound(SoundBuffer buf) {
	for (int i = 0; i < SOUNDSMAX; i++)
	{
		if (sounds[i].getStatus() == Sound::Stopped) {
			buffers[i] = SoundBuffer(buf);
			sounds[i].setBuffer(buffers[i]);
			sounds[i].play();
			return;
		}
	}
}

void Game::SetDeltaTime(float deltaTime) {
	this->elapsedTime += deltaTime * timeFactor;
	this->deltaTime = deltaTime * timeFactor;
}

float Game::GetElapsedTime() {
	return elapsedTime;
}

void Game::SetScoreText(int score) {
	scoreText.setString(std::to_string(score));
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

bool Game::GameOver() {
	if (gameOver && timeFactor == 0.0f) {
		return true;
	}
	return false;
}

void Game::SetVolume(float vol) {
	volume = vol;
	music.setVolume(volume);
	for (int i = 0; i < 50; i++)
	{
		sounds[i].setVolume(volume / 1.5f);
	}
}

void Game::SetBoard(Board* board) {
	delete this->board;
	this->board = board;
}