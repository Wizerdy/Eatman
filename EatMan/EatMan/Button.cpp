#include "Button.h"

void Button::ChangeBtnState(BtnState state)
{
	switch (state)
	{
	case NORMAL:
		this->state = BtnState::NORMAL;
		text.setFillColor(background);
		break;
	case PRESSED:
		this->state = BtnState::PRESSED;
		text.setFillColor(Color(background.r / 3, background.g / 3, background.b / 3, background.a));
		break;
	case HIGHLIGHT:
		this->state = BtnState::HIGHLIGHT;
		text.setFillColor(Color(background.r / 2, background.g / 2, background.b / 2, background.a));
		break;
	case DISABLE:
		this->state = BtnState::DISABLE;
		text.setFillColor(Color(background.r / 5, background.g / 5, background.b / 5, background.a));
		break;
	default:
		break;
	}
}

Button::Button(int posX, int posY, int sizeY, BtnState state, Color background, String stringText)
{
	this->posX = posX;
	this->posY = posY;
	//this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->state = state;
	this->background = background;

	font.loadFromFile(getAssetsPath() + "Assets/font.ttf");

	text.setFont(font);
	text.setCharacterSize(sizeY);
	text.setString(stringText);
	this->sizeX = text.getLocalBounds().width;
	shape.setOrigin(sizeX / 2, sizeY / 2);
	shape.setPosition(posX, posY);
	shape.setFillColor(Color(50, 200, 125, 0));

	text.setOrigin(shape.getOrigin().x, shape.getOrigin().y + sizeY / 5);
	text.setPosition(shape.getPosition().x, shape.getPosition().y);
	text.setFillColor(background);

	
	shape.setSize(Vector2f(text.getLocalBounds().width + 5, text.getCharacterSize()));
}

Button::~Button()
{
}

void Button::Draw(RenderWindow* window)
{
	window->draw(shape);
	window->draw(text);
}

bool Button::Touched()
{
	if (state != BtnState::DISABLE) {
		if (Mouse::getPosition().x > posX - sizeX / 2 &&
			Mouse::getPosition().y > posY - sizeY / 2 &&
			Mouse::getPosition().x < posX + sizeX / 2 &&
			Mouse::getPosition().y < posY + sizeY / 2
		) {
			if (Mouse::isButtonPressed(Mouse::Left)) {
				ChangeBtnState(BtnState::PRESSED);
			}
			else if (state == BtnState::PRESSED && !Mouse::isButtonPressed(Mouse::Left)) {
				ChangeBtnState(BtnState::NORMAL);
				return true;
			}
			else {
				ChangeBtnState(BtnState::HIGHLIGHT);
			}
		}
		else {
			ChangeBtnState(BtnState::NORMAL);
		}
	}
	return false;
}

RectangleShape Button::GetShape() {
	return shape;
}
