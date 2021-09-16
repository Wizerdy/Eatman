#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>

#define MAXWAVES 20
#define WAVETRAIL 5
#define WAVESPEED 0.01f

using namespace std;
using namespace sf;

int Superior(int a, int b);
int Inferior(int a, int b);

struct Wave {
	int index;
	Color color;

	Wave(int i, Color col) : index(i), color(col) {}
};

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

		void WaveMovement(float elapsedTime);

	public:
		Board(float sizeX, float sizeY, float shapeSize, float shapeDelta, Color color, bool outline = false, int outlineThickness = 2);
		~Board();

		void Draw(RenderWindow* window);
		void Act(float elapsedTime);
		void Wave(Color color);

		Color GetColor();
};
