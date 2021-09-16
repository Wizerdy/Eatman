// EatMan.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SFML/Audio.hpp>

#include "Game.h"
#include "Board.h"

int main()
{
    srand(time(NULL));

    sf::Music music;
    music.openFromFile(getAssetsPath() + "Assets/EatManMusicV2.wav");
    music.play();

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    //sf::RenderWindow window(sf::VideoMode(500, 500), "ChronoSpacer");
    //sf::RenderWindow window(sf::VideoMode(1280, 720), "ChronoSpacer", Style::None, settings);
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "ChronoSpacer", sf::Style::Fullscreen, settings);

    // Initialise everything below
    Clock clock;

    Game* game = new Game(&window);
    //game->SpawnEnemy(5, 5);
    //game->SpawnEnemy(500, 500);
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            //game->SpawnEnemy(i * ENEMYSIZE * 3.0f, j * ENEMYSIZE * 3.0f);
        }
    }

    // Game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) {
                    window.close();
                }
                else if (event.key.code == Keyboard::R) {
                    delete game;
                    game = new Game(&window);
                }
            }
            else if (event.type == Event::MouseButtonPressed) {
                //game->Wave(Color(rand() % 125, rand() % 125, rand() % 125, 255));
                if (event.mouseButton.button == Mouse::Button::Left) {
                    game->Shoot((float)Mouse::getPosition(window).x, (float)Mouse::getPosition(window).y);
                }
            }
        }

        window.clear();

        game->Draw();
        window.display();

        game->Act();

        if (music.getStatus() == music.Stopped) {
            music.openFromFile(getAssetsPath() + "Assets/EatManMusicLoop.wav");
            music.play();
            music.setLoop(true);
        }


        float deltaTime = clock.getElapsedTime().asSeconds();
        game->SetDeltaTime(deltaTime);
        clock.restart();

    }
}

// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
