// GUI/scenes/GameOverScene.hpp
#pragma once

#include "Scene.hpp"
#include "../components/Button.hpp"
#include "../game/GameController.hpp"
#include <memory>
#include <vector>

namespace coup {
    namespace gui {

        class GameOverScene : public Scene {
        private:
            sf::Sprite background;
            GameController* gameController;

            // Display elements
            sf::Text gameOverText;
            sf::Text winnerText;
            sf::Text statsTitle;
            std::vector<sf::Text> playerStats;

            // Buttons
            std::unique_ptr<Button> playAgainButton;
            std::unique_ptr<Button> mainMenuButton;

            // Animation
            float animationTimer = 0.0f;

        public:
            GameOverScene(GameController* controller);

            void onEnter() override;
            void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
            void update(float deltaTime, const sf::RenderWindow& window) override;
            void render(sf::RenderWindow& window) override;

        private:
            void createPlayerStats();
        };

    } // namespace gui
} // namespace coup