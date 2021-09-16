#pragma once

#include <iostream>
#include "Game.h"
#include "Button.h"

using namespace std;

enum class MnState {
	HOME,
	GAME,
	RESTART,
	SETTINGS
};

class Menu
{
	private:
		Font font;
		Text title;

		Button** buttons;
		RenderWindow* window;
		RectangleShape background;

		Text** texts;

		int volume;
		int boardId;

		Board** boards;

		MnState scene;

	public:
		Game* game;

		Menu(RenderWindow* window);
		~Menu();

		void Draw();
		void Act();

		void RestartGame();
		void SetScene(MnState scene);
		MnState GetScene();
		bool IsInsideGame();
};