// GUI/scenes/Scene.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace coup {
    namespace gui {

        class SceneManager; // Forward declaration

        // Base class for all scenes in the game
        class Scene {
            friend class SceneManager; // Allow SceneManager to access finished state

        public:
            Scene() = default;
            virtual ~Scene() = default;

            // Called when scene becomes active
            virtual void onEnter() {}

            // Called when scene becomes inactive
            virtual void onExit() {}

            // Handle input events
            virtual void handleEvent(const sf::Event& event, const sf::RenderWindow& window) = 0;

            // Update logic (animations, timers, etc.)
            virtual void update(float deltaTime, const sf::RenderWindow& window) = 0;

            // Render the scene
            virtual void render(sf::RenderWindow& window) = 0;

            // Check if scene wants to transition to another
            virtual bool isFinished() const { return finished; }

            // Get the next scene to transition to
            virtual std::string getNextScene() const { return nextScene; }

        protected:
            bool finished = false;
            std::string nextScene;

            // Helper to transition to another scene
            void transitionTo(const std::string& sceneName) {
                nextScene = sceneName;
                finished = true;
            }
        };

    } // namespace gui
} // namespace coup