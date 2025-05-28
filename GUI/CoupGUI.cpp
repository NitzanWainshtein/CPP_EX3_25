// GUI/CoupGUI.cpp
#include "CoupGUI.hpp"
#include "scenes/MainMenuScene.hpp"
#include "scenes/PlayerCountScene.hpp"
#include "scenes/PlayerNamesScene.hpp"
#include "scenes/GameScene.hpp"
#include "scenes/GameOverScene.hpp"
#include <iostream>

namespace coup {
namespace gui {

CoupGUI::CoupGUI()
    : window(sf::VideoMode(1280, 900), "Coup Game",
             sf::Style::Titlebar | sf::Style::Close) {
    window.setFramerateLimit(60);
}

bool CoupGUI::initialize() {
    // Load resources
    auto& rm = ResourceManager::getInstance();
    if (!rm.loadAllResources()) {
        std::cerr << "Failed to load resources!" << std::endl;
        return false;
    }

    // Create managers
    sceneManager = std::make_unique<SceneManager>();
    gameController = std::make_unique<GameController>();

    // Setup scenes
    setupScenes();

    // Start with main menu
    sceneManager->switchTo("MainMenu");

    return true;
}

void CoupGUI::setupScenes() {
    // Create and register all scenes
    sceneManager->registerScene<MainMenuScene>("MainMenu");

    auto playerCountScene = std::make_shared<PlayerCountScene>();
    sceneManager->registerSceneInstance("PlayerCount", playerCountScene);

    auto playerNamesScene = std::make_shared<PlayerNamesScene>(gameController.get());
    sceneManager->registerSceneInstance("PlayerNames", playerNamesScene);

    auto gameScene = std::make_shared<GameScene>(gameController.get());
    sceneManager->registerSceneInstance("Game", gameScene);

    auto gameOverScene = std::make_shared<GameOverScene>(gameController.get());
    sceneManager->registerSceneInstance("GameOver", gameOverScene);

    // Setup scene connections
    sceneManager->setSceneConnection("PlayerCount", "PlayerNames",
        [playerCountScene, playerNamesScene]() {
            playerNamesScene->setPlayerCount(playerCountScene->getSelectedCount());
        });

    sceneManager->setSceneConnection("PlayerNames", "MainMenu",
        [this]() {
            gameController->resetGame();
        });

    sceneManager->setSceneConnection("Game", "MainMenu",
        [this]() {
            gameController->resetGame();
        });
}

void CoupGUI::run() {
    while (window.isOpen() && running) {
        float deltaTime = clock.restart().asSeconds();

        handleEvents();
        update(deltaTime);
        render();

        // Check if we need to exit
        if (sceneManager->getCurrentSceneName() == "Exit" ||
            !sceneManager->getCurrentScene()) {
            window.close();
        }
    }
}

void CoupGUI::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return;
        }

        sceneManager->handleEvent(event, window);
    }
}

void CoupGUI::update(float deltaTime) {
    sceneManager->update(deltaTime, window);
    sceneManager->checkTransition();
}

void CoupGUI::render() {
    window.clear();
    sceneManager->render(window);
    window.display();
}

} // namespace gui
} // namespace coup