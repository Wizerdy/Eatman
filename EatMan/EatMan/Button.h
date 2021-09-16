#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "Utilities.h"

using namespace sf;
using namespace std;

enum BtnState {
	NORMAL,
	PRESSED,
	HIGHLIGHT,
	DISABLE
};

class Button
{
private:
	int posX, posY;
	int sizeX, sizeY;
	RectangleShape shape;
	BtnState state;
	Text text;
	Font font;
	Color background;

	void ChangeBtnState(BtnState state);
public:
	Button(int posX, int posY, int sizeY, BtnState state, Color background, String stringText);
	~Button();

	void Draw(RenderWindow* window);
	bool Touched();

	RectangleShape GetShape();
};