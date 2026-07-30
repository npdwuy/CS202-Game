#include "GameManager.hpp"
#include "MenuState.hpp"

int main() {
    auto& game = GameManager::getInstance();
    game.pushState(std::make_unique<MenuState>());
    game.run();
    return 0;
}