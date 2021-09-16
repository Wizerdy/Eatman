#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SFML/Audio.hpp>

#include "Game.h"
#include "Board.h"
#include "Menu.h"

int main()
{
    srand(time(NULL));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    //sf::RenderWindow window(sf::VideoMode(500, 500), "ChronoSpacer");
    //sf::RenderWindow window(sf::VideoMode(1280, 720), "ChronoSpacer", Style::None, settings);
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "ChronoSpacer", sf::Style::Fullscreen, settings);

    // Initialise everything below
    Clock clock;

    Menu* menu = new Menu(&window);

    // Game loop
    while (window.isOpen()) {
        menu->Act();

        if (menu->IsInsideGame()) {
            menu->game->Act();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) {
                    if (menu->GetScene() != MnState::HOME) {
                        menu->SetScene(MnState::HOME);
                    }
                    else {
                        window.close();
                    }
                }
                else if (menu->IsInsideGame() && event.key.code == Keyboard::R) {
                    menu->RestartGame();
                }
            }
            else if (event.type == Event::MouseButtonPressed) {
                //game->Wave(Color(rand() % 125, rand() % 125, rand() % 125, 255));
                //game->Impact((float)Mouse::getPosition(window).x, (float)Mouse::getPosition(window).y, Color::Red);
                if (menu->IsInsideGame() && event.mouseButton.button == Mouse::Button::Left) {
                    menu->game->Shoot((float)Mouse::getPosition(window).x, (float)Mouse::getPosition(window).y);
                }
            }
        }

        window.clear();


        if (menu->IsInsideGame()) {
            menu->game->Draw();
        }
        menu->Draw();

        window.display();

        if (menu->IsInsideGame()) {
            float deltaTime = clock.getElapsedTime().asSeconds();
            menu->game->SetDeltaTime(deltaTime);
        }
        clock.restart();

    }
}