#include "Particles.h"

Particle::Particle(float posX, float posY, float size, float speed, float lifeTime) : Character(posX, posY, size, speed) {
	this->lifeTime = lifeTime;

	birthFrame = -1.0f;
	dead = false;
}

Particle::~Particle() {

}

void Particle::Draw(RenderWindow* window) {
	window->draw(shape);
}

void Particle::Act(float elapsedTime, float deltaTime) {
	if (birthFrame == -1.0f) {
		birthFrame = elapsedTime;
	}
	else if (elapsedTime - birthFrame > lifeTime) {
		dead = true;
	}
	else {
		Move(elapsedTime, deltaTime);
		Color color = shape.getFillColor();
		float delta = (lifeTime - (elapsedTime - birthFrame)) / (lifeTime/1.6f);
		if (delta > 1) {
			delta = 1;
		}
		SetShapeColor(Color(color.r, color.g, color.b, color.a * delta));
	}
}

void Particle::Move(float elapsedTime, float deltaTime) {
	//SetPos(posX + dirX * speed * deltaTime, posY + dirY * speed * deltaTime);
	float time = lifeTime / (((elapsedTime - birthFrame) + 0.001f) * 2.5f) * deltaTime;
	//float time = 1 / (lifeTime / ((elapsedTime - birthFrame) + 0.001f)) * deltaTime * 5.0f;
	SetPos(GetPosX() + dirX * speed * time, GetPosY() + dirY * speed * time);
}

bool Particle::IsDead() {
	return dead;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

GroupParticles::GroupParticles(float number, float size, float speed, float lifeTime, float posX, float posY, float dirX, float dirY, float spray, Color color) {
	float ang = atan(dirY / dirX) * RADTODEG;
	if (dirX < 0) {
		ang += 180;
	}
	for (int i = 0; i < number; i++)
	{
		particles.push_back(new Particle(posX, posY, size, speed, lifeTime));
		float angRand = Rand(ang - spray/2, ang + spray/2);
		particles.back()->SetDir(cos(angRand / RADTODEG), sin(angRand / RADTODEG));
		particles.back()->SetShapeColor(color);
	}

	dead = false;
}

GroupParticles::GroupParticles(float number, float sizeMin, float sizeMax, float speedMin, float speedMax, 
	float lifeTimeMin, float lifeTimeMax, float posX, float posY, float dirX, float dirY, float spray, Color color)
{
	float ang = atan(dirY / dirX) * RADTODEG;
	if (dirX < 0) {
		ang += 180;
	}

	for (int i = 0; i < number; i++)
	{
		particles.push_back(new Particle(posX, posY, Rand(sizeMin, sizeMax), Rand(speedMin, speedMax), Rand(lifeTimeMin, lifeTimeMax)));
		float angRand = Rand(ang - spray/2, ang + spray/2);
		particles.back()->SetDir(cos(angRand / RADTODEG), sin(angRand / RADTODEG));
		particles.back()->SetShapeColor(color);
	}

	dead = false;
}

GroupParticles::~GroupParticles() {
	for(Particle* particle : particles)
	{
		delete particle;
	}
}

void GroupParticles::Draw(RenderWindow* window) {
	for (Particle* particle : particles) {
		particle->Draw(window);
	}
}

void GroupParticles::Move(float deltaTime) {
	for (Particle* particle : particles) {
		//particle->Move(deltaTime);
	}
}

void GroupParticles::Act(float elapsedTime, float deltaTime) {
	for (list<Particle*>::iterator particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle)->Act(elapsedTime, deltaTime);

		if ((*particle)->IsDead()) {
			delete (*particle);
			particle = particles.erase(particle);

			if (particle == particles.end()) {
				if (particles.size() == 0) {
					dead = true;
				}
				break;
			}
		}
	}
}

bool GroupParticles::IsDead() {
	return dead;
}

float Rand(float min, float max) {
	return (float)rand() / (float)(RAND_MAX / (max - min)) + min;
}

int RandInt(int min, int max) {
	return rand() % (min - max) + min;
}