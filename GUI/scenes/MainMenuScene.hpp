// GUI/scenes/MainMenuScene.hpp
#pragma once

#include "Scene.hpp"
#include "../components/Button.hpp"
#include <memory>
#include <vector>

namespace coup {
    namespace gui {

        class MainMenuScene : public Scene {
        private:
            sf::Sprite background;
            sf::Text titleText;
            sf::Text creditsText;
            std::vector<std::unique_ptr<Button>> buttons;

        public:
            MainMenuScene();

            void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
            void update(float deltaTime, const sf::RenderWindow& window) override;
            void render(sf::RenderWindow& window) override;
        };

    } // namespace gui
} // namespace coup