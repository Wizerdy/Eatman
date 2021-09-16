#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <typeinfo>

#include "Utilities.h"

#define MAXWAVES 20
#define WAVETRAIL 7
#define WAVESPEED 0.01f

#define IMPACTMAXRADIUS 15

using namespace std;
using namespace sf;

struct Wave;
struct Impact;

class Board {
	private:
		Vector2f size;
		Vector2f gridSize;
		float shapeSize;
		float shapeDelta;
		Color color;
		bool outline;
		list<CircleShape> shapes;

		float waveFrame;
		Wave* waves[MAXWAVES] = {};
		Impact* impact[MAXWAVES] = {};

		void WaveMovement();
		void ImpactMovement();

		void SetShapeColor(CircleShape* shape, Color color);
		Color GetShapeColor(CircleShape* shape);

	public:
		Board(float sizeX, float sizeY, float shapeSize, float shapeDelta, Color color, bool outline = false, int outlineThickness = 2);
		Board(Board* board);
		~Board();

		void Draw(RenderWindow* window);
		void Act(float elapsedTime);
		void Wave(Color color);
		void Impact(float posX, float posY, Color color);

		Color GetColor();
		Vector2f GetSize();
		Vector2f GetGridSize();
		float GetShapeSize();
		list<CircleShape>* GetShapes();
};

struct Wave {
	int index;
	Color color;

	Wave(int i, Color col) : index(i), color(col) {}
};

struct Impact {
	bool active;
	Vector2f center;
	float radius;
	Color color;
	CircleShape debug;

	Impact(Vector2f center, float radius, Color color) : center(center), radius(radius), color(color), active(false) {}
	Impact(float posX , float posY, float radius, Color color) : center(Vector2f(posX, posY)), radius(radius), color(color), active(false) {}
};