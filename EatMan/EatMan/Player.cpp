#include "Player.h"

Player::Player() {
	bullet = new Bullet(0, 0, BULLETSIZE, BULLETSPEED);
	bullet->isActive = false;
	infected = nullptr;
	bullet->SetShapeColor(Color(0, CHARCOLOR.g, 0, CHARCOLOR.a));

	damageTime = 0.0f;
	deadTime = 0.0f;
}

Player::~Player() {
	delete bullet;
}

void Player::Shoot(float x, float y) {
	x = x - infected->GetPosX();
	y = y - infected->GetPosY();
	float mag = sqrt(x * x + y * y);

	bullet->SetPos(
		infected->GetPosX() + (x * (infected->GetSize() + BULLETSIZE) / mag),
		infected->GetPosY() + (y * (infected->GetSize() + BULLETSIZE) / mag)
	);
	bullet->SetDir(x/mag, y/mag);
	bullet->isActive = true;
}

void Player::Infect(Enemy* enemy) {
	infected = enemy;
	deadTime = 0;
}

bool Player::Act(float time, bool* gameOver) {
	if (HasInfected() && !bullet->isActive) {
		if (time - damageTime > 1) {
			if (infected->GetLife() > 0) {
				DamageInfected(2, time);
				damageTime = time;
				return true;
			}
		}

		if (deadTime != 0 && time - deadTime > DYINGTIME) {
			//infected->SetShapeColor(Color(CHARCOLOR.r / 2.0f, CHARCOLOR.g / 1.5f, CHARCOLOR.b/1.5f, CHARCOLOR.a));
			(*gameOver) = true;
		}
	}
	return false;
}

void Player::DamageInfected(int damage, float time) {
	infected->SetLife(infected->GetLife() - damage);

	if (deadTime == 0 && infected->GetLife() <= 0) {
		deadTime = time;
		infected->SetShapeColor(Color(CHARCOLOR.r / 2.3f, 0, CHARCOLOR.b/2.7f, CHARCOLOR.a));
	}
}

void Player::DeleteInfected() {
	delete infected;
	Infect(nullptr);
}

Enemy* Player::GetInfected() {
	return infected;
}

Bullet* Player::GetBullet() {
	return bullet;
}

bool Player::HasInfected() {
	if (infected == nullptr) {
		return false;
	}
	return true;
}

bool Player::IsInsideInfected() {
	if (HasInfected() && !bullet->isActive) {
		return true;
	}
	return false;
}

bool Player::IsInfected(Enemy* enemy) {
	if (HasInfected() && enemy == infected) {
		return true;
	}
	return false;
}