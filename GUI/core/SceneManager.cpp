// GUI/core/SceneManager.cpp
#include "SceneManager.hpp"
#include <iostream>

namespace coup {
    namespace gui {

        void SceneManager::switchTo(const std::string& sceneName) {
            // Exit current scene
            if (currentScene) {
                currentScene->onExit();

                // Execute connection callback if exists
                auto fromIt = connections.find(currentSceneName);
                if (fromIt != connections.end()) {
                    auto toIt = fromIt->second.find(sceneName);
                    if (toIt != fromIt->second.end()) {
                        toIt->second();
                    }
                }
            }

            // Find and activate new scene
            auto it = scenes.find(sceneName);
            if (it != scenes.end()) {
                currentScene = it->second.get();
                currentSceneName = sceneName;
                currentScene->onEnter();
            } else {
                std::cerr << "Scene not found: " << sceneName << std::endl;
            }
        }

        void SceneManager::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
            if (currentScene) {
                currentScene->handleEvent(event, window);
            }
        }

        void SceneManager::update(float deltaTime, const sf::RenderWindow& window) {
            if (currentScene) {
                currentScene->update(deltaTime, window);
            }
        }

        void SceneManager::render(sf::RenderWindow& window) {
            if (currentScene) {
                currentScene->render(window);
            }
        }

        void SceneManager::checkTransition() {
            if (currentScene && currentScene->isFinished()) {
                std::string nextScene = currentScene->getNextScene();

                // Reset finished state
                currentScene->finished = false;

                // Special case for Exit
                if (nextScene == "Exit") {
                    currentScene = nullptr;
                    return;
                }

                switchTo(nextScene);
            }
        }

    } // namespace gui
} // namespace coup
