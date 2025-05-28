// GUI/core/SceneManager.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include "../scenes/Scene.hpp"

namespace coup {
    namespace gui {

        class SceneManager {
        private:
            std::unordered_map<std::string, std::shared_ptr<Scene>> scenes;
            Scene* currentScene = nullptr;
            std::string currentSceneName;

            // Scene transition callbacks
            std::unordered_map<std::string, std::unordered_map<std::string, std::function<void()>>> connections;

        public:
            SceneManager() = default;

            // Register a scene by creating it
            template<typename T, typename... Args>
            void registerScene(const std::string& name, Args&&... args) {
                scenes[name] = std::make_shared<T>(std::forward<Args>(args)...);
            }

            // Register an existing scene instance
            void registerSceneInstance(const std::string& name, std::shared_ptr<Scene> scene) {
                scenes[name] = scene;
            }

            // Set a callback for scene transitions
            void setSceneConnection(const std::string& from, const std::string& to,
                                   std::function<void()> callback) {
                connections[from][to] = callback;
            }

            // Switch to a different scene
            void switchTo(const std::string& sceneName);

            // Get current scene
            Scene* getCurrentScene() { return currentScene; }
            const std::string& getCurrentSceneName() const { return currentSceneName; }

            // Update current scene
            void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
            void update(float deltaTime, const sf::RenderWindow& window);
            void render(sf::RenderWindow& window);

            // Check if current scene wants to transition
            void checkTransition();
        };

    } // namespace gui
} // namespace coup