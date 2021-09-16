#include "InstantText.h"


InstantText::InstantText(float posX, float posY, string text, Font font, int charSize, Color color, float elapsedTime) {
	pos = Vector2f(posX, posY);
	targetPos = pos;

	this->font = font;
	this->text.setFont(this->font);
	this->text.setString(text);
	this->text.setFillColor(color);
	this->text.setPosition(pos);
	this->text.setCharacterSize(charSize);

	birthFrame = elapsedTime;

	dead = false;
}

InstantText::~InstantText() {
}

void InstantText::Draw(RenderWindow* window) {
	window->draw(text);
}

void InstantText::Act(float elapsedTime) {
	if (elapsedTime - birthFrame > TEXTDURATION) {
		dead = true;
	}
}

void InstantText::Move(float deltaTime) {
	if (targetPos.y != pos.y) {
		if (abs(pos.y - targetPos.y) < TEXTSPEED * deltaTime) {
			pos.y = targetPos.y;
		}
		else {
			pos.y += TEXTSPEED * Sign(targetPos.y, pos.y) * deltaTime;
		}
		SetPos(pos.x, pos.y);
	}
	if (targetPos.x != pos.x) {
		if (abs(pos.x - targetPos.x) < TEXTSPEED * deltaTime) {
			pos.x = targetPos.x;
		}
		else {
			pos.x += TEXTSPEED * Sign(targetPos.x, pos.x) * deltaTime;
		}
		SetPos(pos.x, pos.y);
	}
}

void InstantText::MoveTo(float posX, float posY) {
	targetPos = Vector2f(posX, posY);
}

void InstantText::MoveTo(Vector2f pos) {
	MoveTo(pos.x, pos.y);
}

void InstantText::SetPos(float posX, float posY) {
	pos = Vector2f(posX, posY);
	this->text.setPosition(pos);
}

void InstantText::SetPos(Vector2f pos) {
	SetPos(pos.x, pos.y);
}

Vector2f InstantText::GetPos() {
	return pos;
}

Vector2f InstantText::GetTargetPos() {
	return targetPos;
}

bool InstantText::IsDead() {
	return dead;
}

// ------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------

InstantTextList::InstantTextList(float posX, float posY, Font font, int charSize, Color color) {
	pos = Vector2f(posX, posY);
	this->font = font;
	this->charSize = charSize;
	this->color = color;
}

InstantTextList ::~InstantTextList() {
	for (InstantText* text : texts)
	{
		delete text;
	}
}

void InstantTextList::Draw(RenderWindow* window) {
	for (InstantText* text : texts) {
		text->Draw(window);
	}
}

void InstantTextList::Act(float elapsedTime, float deltaTime) {
	for (list<InstantText*>::iterator text = texts.begin(); text != texts.end(); text++)
	{
		(*text)->Act(elapsedTime);

		if ((*text)->IsDead()) {
			if (text != texts.end() && texts.size() > 1) {
				for (list<InstantText*>::iterator nextText = prev(texts.end()); nextText != text; nextText--)
				{
					(*nextText)->MoveTo((*prev(nextText))->GetTargetPos());
				}
			}

			delete (*text);
			text = texts.erase(text);

			if (text == texts.end()) {
				break;
			}
		}

		(*text)->Move(deltaTime);
	}
}

void InstantTextList::AddText(float elapsedTime, string text) {
	texts.push_back(new InstantText(pos.x, pos.y + charSize * texts.size(), text, font, charSize, color, elapsedTime));
	texts.back()->MoveTo(pos.x + 10, texts.back()->GetPos().y);
}

void InstantTextList::AddHeadshot(float elapsedTime) {
	AddText(elapsedTime, HEADSHOT);
}
void InstantTextList::AddBodyshot(float elapsedTime) {
	AddText(elapsedTime, BODYSHOT);
}
void InstantTextList::AddLegshot(float elapsedTime) {
	AddText(elapsedTime, LEGSHOT);
}
void InstantTextList::AddArmshot(float elapsedTime) {
	AddText(elapsedTime, ARMSHOT);
}

void InstantTextList::AddShot(float elapsedTime, int index) {
	switch (index)
	{
		case 0:
			AddLegshot(elapsedTime);
			break;
		case 1:
			AddArmshot(elapsedTime);
			break;
		case 2:
			AddBodyshot(elapsedTime);
			break;
		case 3:
			AddHeadshot(elapsedTime);
			break;
	}
}