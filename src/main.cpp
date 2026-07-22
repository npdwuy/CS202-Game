#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include "GameState.hpp"
#include "MenuState.hpp"
#include "PlayState.hpp"

int main() {
    // Resolution
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "CS202-Group5");

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    // FPS
    const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);

    // current game state
    std::unique_ptr<GameState> currentState = std::make_unique<MenuState>();

    while(window.isOpen()) {
        sf::Time elapsedTime = clock.restart();

        if(elapsedTime > sf::seconds(0.25f)) {
            elapsedTime = sf::seconds(0.25f);
        }

        timeSinceLastUpdate += elapsedTime;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Input handling here
            if(currentState != nullptr) currentState->Input(event);
        }


        // Update State - fixed time stamp for better event handling
        while(timeSinceLastUpdate >= TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;

            // Update game logic
            if(currentState != nullptr) currentState->Update(TimePerFrame);
        }

        // Apply state transition if requested
        if (currentState != nullptr && currentState->hasNextState()) {
            currentState = currentState->getNextState();
        }

        window.clear(sf::Color::Black);

        // Render here
        if(currentState != nullptr) currentState->Render(window);

        window.display();
    }

    return 0;
}