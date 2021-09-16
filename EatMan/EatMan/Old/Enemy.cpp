#include "Enemy.h"

Enemy::Enemy(float posX, float posY, float size, float speed, float reloadTime, float shootRange) : Character(posX, posY, size, speed) {
    this->shootRange = shootRange;
    this->reloadTime = reloadTime;
    
    life = 10;
    speedFactor = 1;
    chronoShoot = -reloadTime;
    canShoot = false;

    target = nullptr;

    behaviour = Behaviour::COMING;

    states[State::FEAR] = false;
    states[State::SLOW] = false;

    font.loadFromFile(getAssetsPath() + "Assets/font.ttf");

    lifeText.setFont(font);
    lifeText.setCharacterSize(size * 1.5f);
    lifeText.setFillColor(Color(20, 20, 20, 255));
    lifeText.setOrigin(size / 2, size / 2);
    lifeText.setPosition(posX, posY);

    lifeText.setString(to_string(life));
}

Enemy::~Enemy() {
	for (Bullet* bullet : bullets) {
		if (!bullet->isActive) {
			delete(bullet);
		}
	}
}

void Enemy::Draw(RenderWindow* window) {
    Character::Draw(window);
    //window->draw(lifeText);
}


void Enemy::Move(float deltaTime, float speedFactor) {
	if (target != nullptr && (target->GetPosX() != posX || target->GetPosY() != posY)) {
		//SetPos(posX + dirX * speed * speedFactor * deltaTime, posY + dirY * speed * speedFactor * deltaTime);
        Character::Move(deltaTime * speedFactor);
        lifeText.setPosition(posX, posY);
	}
}

void Enemy::MoveTo(Character* character) {
    behaviour = Behaviour::COMING;
    target = character;

	float x = target->GetPosX() - this->posX;
	float y = target->GetPosY() - this->posY;
	float mag = sqrt(x * x + y * y);

	SetDir(x/mag, y/mag);
}

void Enemy::FleeFrom(Character* character) {
    behaviour = Behaviour::FLEEING;
    target = character;

    float x = target->GetPosX() - this->posX;
    float y = target->GetPosY() - this->posY;
    float mag = sqrt(x * x + y * y);

    SetDir(-x/mag, -y/mag);
}

void Enemy::StopMove() {
    behaviour = Behaviour::MOTIONLESS;
}

void Enemy::Act(float deltaTime) {
    switch (behaviour)
    {
    case Behaviour::COMING:
        Move(deltaTime, 1 * speedFactor);
        break;
    case Behaviour::MOTIONLESS:
        break;
    case Behaviour::FLEEING:
        Move(deltaTime, 1.2f * speedFactor);
        break;
    }
}

int Enemy::GetLife() {
    return life;
}

float Enemy::GetShootRange() {
    return shootRange;
}

void Enemy::SetLife(int life) {
    this->life = life;

    if (life < 0) {
        life = 0;
    }

    SetShapeColor(Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, CHARCOLOR.a));
    lifeText.setString(to_string(life));
}

void Enemy::SetShapeColor(Color color) {
    color = Color(color.r, color.g, color.b, color.a * (life + 5) / 15.0f);
    Character::SetShapeColor(color);
}

Body Enemy::Wound() {
    int random = rand() % 100;

    if (random < 5) {
        return Body::HEAD;
    }
    else if (random < 70) {
        return Body::BELLY;
    }
    else if (random < 85) {
        return Body::ARMS;
    }
    else if (random < 100) {
        return Body::LEGS;
    }
}

void Enemy::Wounded(Body body) {
    switch (body)
    {
    case Body::HEAD:
        life = 0;
        break;
    case Body::BELLY:
        life -= 4;
        break;
    case Body::ARMS:
        life -= 1;
        states[State::FEAR] = true;
        if (HasTarget()) {
            FleeFrom(target);
        }
        break;
    case Body::LEGS:
        life -= 1;
        if (!states[State::SLOW]) {
            states[State::SLOW] = true;
            speedFactor /= 2;
        }
        break;
    }
    SetLife(life);
}

void Enemy::Shoot(float x, float y) {
    x = x - posX;
    y = y - posY;
    float mag = sqrt(x * x + y * y);

	bullets.push_back(new Bullet(this->posX + (x * (size + BULLETSIZE) / mag), this->posY + (y * (size + BULLETSIZE) / mag), BULLETSIZE, BULLETSPEED));
    Bullet* bullet = bullets.back();

    bullet->SetDir(x / mag, y / mag);
}

void Enemy::Shoot(float x, float y, float time) {
    if (canShoot) {
        chronoShoot = time;
        Shoot(x, y);
    }
}

float Enemy::GetChronoShoot() {
    return chronoShoot;
}

float Enemy::GetReloadTime() {
    return reloadTime;
}

void Enemy::SetChronoShoot(float chrono) {
    chronoShoot = chrono;
}

Character* Enemy::GetTarget() {
    return target;
}

bool Enemy::HasTarget() {
    if (target == nullptr) {
        return false;
    }
    return true;
}

void Enemy::SetTarget(Character* character) {
    if (behaviour == Behaviour::MOTIONLESS) {
        canShoot = false;
        SetShapeColor(CHARCOLOR);
    }

    if (states[State::FEAR]) {
        FleeFrom(character);
        return;
    }

    if (Collision(character, GetShootRange() * ENEMYRANGEFACTOR)) {
        FleeFrom(character);
        return;
    }
    else if (Collision(character, GetShootRange())) {
        StopMove();
        return;
    }
    else {
        MoveTo(character);
        return;
    }
}