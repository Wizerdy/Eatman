#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>

#include "Utilities.h"

#define TEXTDURATION 1.2f
#define TEXTSPEED 500

#define HEADSHOT "HeadShot"
#define BODYSHOT "Shot"
#define LEGSHOT "LegShot"
#define ARMSHOT "ArmShot"

using namespace std;
using namespace sf;

class InstantText {
	private:
		Vector2f pos;
		Vector2f targetPos;
		Font font;
		Text text;

		float birthFrame;
		bool dead;

	public:
		InstantText(float posX, float posY, string text, Font font, int charSize, Color color, float elapsedTime);
		~InstantText();

		void Draw(RenderWindow* window);
		void Act(float elapsedTime);
		void Move(float deltaTime);

		void MoveTo(float posX, float posY);
		void MoveTo(Vector2f pos);

		void SetPos(float posX, float posY);
		void SetPos(Vector2f pos);

		Vector2f GetPos();
		Vector2f GetTargetPos();
		bool IsDead();
};

class InstantTextList {
	private:
		Vector2f pos;
		Font font;
		int charSize;
		Color color;

		list<InstantText*> texts;

	public:
		InstantTextList(float posX, float posY, Font font, int charSize, Color color);
		~InstantTextList ();

		void Draw(RenderWindow* window);
		void Act(float elapsedTime, float deltaTime);

		void AddText(float elapsedTime, string text);

		void AddHeadshot(float elapsedTime);
		void AddBodyshot(float elapsedTime);
		void AddLegshot(float elapsedTime);
		void AddArmshot(float elapsedTime);

		void AddShot(float elapsedTime, int index);
};