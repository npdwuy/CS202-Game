#pragma once

#include "ICommand.hpp"
#include "GameManager.hpp"
#include <functional>

// A generic command that executes a given lambda function.
// This is useful for complex, localized UI logic that doesn't need a dedicated class.
class LambdaCommand : public ICommand {
public:
    explicit LambdaCommand(std::function<void()> callback) 
        : m_callback(std::move(callback)) {}

    void execute() override {
        if (m_callback) {
            m_callback();
        }
    }

private:
    std::function<void()> m_callback;
};

// Command to pop the current state from GameManager
class PopStateCommand : public ICommand {
public:
    void execute() override {
        GameManager::getInstance().popState();
    }
};

// Command to exit the game
class ExitGameCommand : public ICommand {
public:
    void execute() override {
        GameManager::getInstance().quit();
    }
};
