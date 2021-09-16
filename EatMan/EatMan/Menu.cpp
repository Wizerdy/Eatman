#include "Menu.h"

Menu::Menu(RenderWindow* window)
{
	this->window = window;

	volume = 25;
	boardId = 0;
	font.loadFromFile(getAssetsPath() + "Assets/font.ttf");

	Button** temp = new Button * [8] {
		new Button(window->getSize().x / 2, window->getSize().y * 5 / 10, 100, BtnState::NORMAL, Color(255, 255, 255, 255), "Play"),
		new Button(window->getSize().x / 2, window->getSize().y * 6 / 10, 100, BtnState::NORMAL, Color(255, 255, 255, 255), "Settings"),
		new Button(window->getSize().x / 2, window->getSize().y * 7 / 10, 100, BtnState::NORMAL, Color(255, 255, 255, 255), "Quit"),
		new Button(window->getSize().x / 2, window->getSize().y * 5 / 10, 100, BtnState::NORMAL, Color(255, 255, 255, 255), "Try again"),
		new Button(window->getSize().x / 10, window->getSize().y / 10, 60, BtnState::NORMAL, Color(255, 255, 255, 255), "Back"),
		new Button(window->getSize().x * 9 / 16, window->getSize().y / 2, 65, BtnState::NORMAL, Color::White , "o"),
		new Button(window->getSize().x * 7 / 16, window->getSize().y / 2, 65, BtnState::NORMAL, Color::White , "o"),
		new Button(window->getSize().x / 2, window->getSize().y * 2 / 3, 65, BtnState::NORMAL, Color::White , "o")
	};

	Text** temp2 = new Text * [5]{
		new Text(to_string(volume), font, 50),
		new Text("Volume", font, 70),
		new Text("Board", font, 70),
	};

	buttons = temp;
	texts = temp2;

	boards = new Board * [3]{
		new Board(window->getSize().x, window->getSize().y, 22, 10, Color(20, 20, 20, 255), true, 3),
		new Board(window->getSize().x, window->getSize().y, 22, 44, Color(20, 20, 20, 255)),
		new Board(window->getSize().x, window->getSize().y, 22, 10, Color(10, 10, 10, 255)),
	};

	texts[0]->setOrigin(texts[0]->getLocalBounds().width/2, texts[0]->getLocalBounds().height / 2);
	texts[0]->setPosition(window->getSize().x / 2, window->getSize().y / 2 - 10);
	
	texts[1]->setOrigin(texts[1]->getLocalBounds().width/2, texts[1]->getLocalBounds().height / 2);
	texts[1]->setPosition(window->getSize().x / 2, window->getSize().y / 2 - texts[0]->getLocalBounds().height*3);

	texts[2]->setOrigin(texts[2]->getLocalBounds().width / 2, texts[2]->getLocalBounds().height / 2);
	texts[2]->setPosition(window->getSize().x / 2, window->getSize().y * 2 / 3 - 85);

	background.setFillColor(Color(0, 0, 0, 72));
	background.setSize(Vector2f(window->getSize().x, window->getSize().y));
	background.setOrigin(window->getSize().x / 2, window->getSize().y / 2);
	background.setPosition(window->getSize().x / 2, window->getSize().y / 2);
	

	title.setFont(font);
	title.setFillColor(Color(255, 255, 255, 255));
	title.setCharacterSize(200);
	title.setString("EatMan");
	title.setPosition(window->getSize().x / 2, window->getSize().y * 2 / 10);
	title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);

	scene = MnState::HOME;

}

Menu::~Menu() {

}

void Menu::Draw() {
	switch (scene)
	{
	//Menu principal
	case MnState::HOME:
		window->draw(title);

		buttons[0]->Draw(window);
		buttons[1]->Draw(window);
		buttons[2]->Draw(window);
		break;
	//Jeu
	case MnState::GAME:
		break;
	//Menu GameOver
	case MnState::RESTART:
		window->draw(background);
		buttons[3]->Draw(window);
		break;
	case MnState::SETTINGS:
		buttons[4]->Draw(window);
		buttons[5]->Draw(window);
		buttons[6]->Draw(window);
		buttons[7]->Draw(window);

		window->draw(*texts[0]);
		window->draw(*texts[1]);
		window->draw(*texts[2]);
		break;
	}
}

void Menu::Act() {
	switch (scene)
	{
		//Menu principal
	case MnState::HOME:
		if (buttons[0]->Touched()) {
			game = new Game(window);
			game->SetVolume(volume);
			//game->SetBoard(boards[boardId]);
			scene = MnState::GAME;
		}
		if (buttons[1]->Touched()) {
			scene = MnState::SETTINGS;
		}
		if (buttons[2]->Touched()) {
			window->close();
		}
		break;
		//Jeu
	case MnState::GAME:
		if (game->GameOver()) {
			scene = MnState::RESTART;
		}
		break;
		//Menu GameOver
	case MnState::RESTART:
		if (buttons[3]->Touched()) {
			RestartGame();
			scene = MnState::GAME;
		}
		break;
	case MnState::SETTINGS:
		if (buttons[4]->Touched()) {
			scene = MnState::HOME;
		}
		if (buttons[5]->Touched() && volume < 99) {
			volume++;
			texts[0]->setString(to_string(volume));
		}
		if (buttons[6]->Touched() && volume > 0) {
			volume--;
			texts[0]->setString(to_string(volume));
		}
		if (buttons[7]->Touched()) {
			boardId = (boardId + 1) % 3;
		}
		break;
	}
}

void Menu::RestartGame() {
	delete game;
	game = new Game(window);
	game->SetVolume(volume);
	//game->SetBoard(boards[boardId]);
	scene = MnState::GAME;
}

MnState Menu::GetScene() {
	return scene;
}

void Menu::SetScene(MnState scene) {
	this->scene = scene;
	if (!IsInsideGame() && game != nullptr) {
		delete game;
		game = nullptr;
	}
}

bool Menu::IsInsideGame() {
	if (scene == MnState::GAME || scene == MnState::RESTART) {
		return true;
	}
	return false;
}