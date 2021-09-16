#include "Board.h"

Board::Board(float sizeX, float sizeY, float shapeSize, float shapeDelta, Color color, bool outline, int outlineThickness) : waves{} {
	size = Vector2f(sizeX, sizeY);
	gridSize = Vector2f(size.x / (shapeSize * 2) + 1 + WAVETRAIL, size.y / (shapeSize * 2) * 1.3f + 1);
	this->color = color;
	this->shapeSize = shapeSize;
	this->shapeDelta = shapeDelta;
	this->outline = outline;

	for (int y = 0; y < gridSize.y; y++)
	{
		for (int x = 0; x < gridSize.x; x++)
		{
			shapes.push_back(CircleShape(shapeSize, 6));
			if (outline) {
				shapes.back().setFillColor(Color::Transparent);
				shapes.back().setOutlineThickness(outlineThickness);
				shapes.back().setOutlineColor(color);
			}
			else {
				shapes.back().setFillColor(color);
			}

			shapes.back().setOrigin(shapeSize, shapeSize);
			float yDelta = sin(0.523599) * shapeSize;
			float xDelta = shapeSize - sin(1.0472) * shapeSize;
			shapes.back().setPosition(x * (shapeSize * 2 - xDelta + shapeDelta) + (shapeSize + shapeDelta/2) * (y % 2), y * (shapeSize * 2 - yDelta + shapeDelta));
		}
	}

	for (int i = 0; i < MAXWAVES; i++)
	{
		waves[i] = new struct Wave(-1, Color::Transparent);
	}

	waveFrame = 0;
}

Board::~Board() {
	for (struct Wave* wave : waves) {
		delete wave;
	}
}

void Board::Draw(RenderWindow* window) {
	for (list<CircleShape>::iterator shape = shapes.begin(); shape != shapes.end(); shape++)
	{
		window->draw( (*shape) );
	}
}

void Board::Act(float elapsedTime) {
	WaveMovement(elapsedTime);

}

void Board::WaveMovement(float elapsedTime) {
	if (elapsedTime - waveFrame > WAVESPEED) {
		for (int i = 0; i < MAXWAVES; i++)
		{
			if (waves[i]->index != -1) {
				list<CircleShape>::iterator it = shapes.begin();
				advance(it, waves[i]->index);

				for (int j = 0; j < gridSize.y; j++)
				{
					// Trail
					for (int delta = 0; delta < WAVETRAIL; delta++)
					{
						if (waves[i]->index - delta > 0) {
							advance(it, -(delta + 1));
							Color color = waves[i]->color;
							color = Color(
								this->color.r + (color.r - this->color.r) * (WAVETRAIL - delta) / (WAVETRAIL + 3),
								this->color.g + (color.g - this->color.g) * (WAVETRAIL - delta) / (WAVETRAIL + 3),
								this->color.b + (color.b - this->color.b) * (WAVETRAIL - delta) / (WAVETRAIL + 3),
								color.a
							);

							if (outline) {
								(*it).setOutlineColor(color);
							}
							else {
								(*it).setFillColor(color);
							}
							advance(it, delta + 1);
						}
					}

					// Cleaner
					if (waves[i]->index - WAVETRAIL + 1 > 0) {
						advance(it, -WAVETRAIL);
						if (outline) {
							(*it).setOutlineColor(color);
						}
						else {
							(*it).setFillColor(color);
						}
						advance(it, WAVETRAIL);
					}

					// Wave
					if (outline) {
						(*it).setOutlineColor(waves[i]->color);
					}
					else {
						(*it).setFillColor(waves[i]->color);
					}

					if (distance(it, shapes.end()) > gridSize.x) {
						advance(it, gridSize.x);
					}
				}

				waves[i]->index++;
				if (waves[i]->index >= gridSize.x) {
					waves[i]->index = -1;
				}
			}
		}
		waveFrame = elapsedTime;
	}
}

void Board::Wave(Color color) {
	for (int i = 0; i < MAXWAVES; i++)
	{
		if(waves[i]->index == -1) {
			waves[i]->index = 0;
			waves[i]->color = color;
			return;
		}
	}
}

Color Board::GetColor() {
	return color;
}

int Superior(int a, int b) {
	if (a > b) {
		return a;
	}
	return b;
}


int Inferior(int a, int b) {
	if (a > b) {
		return b;
	}
	return a;
}
