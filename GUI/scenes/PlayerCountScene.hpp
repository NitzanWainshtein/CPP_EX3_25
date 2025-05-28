// GUI/scenes/PlayerCountScene.hpp
#pragma once

#include "Scene.hpp"
#include "../components/Button.hpp"
#include <memory>
#include <vector>

namespace coup {
    namespace gui {

        class PlayerCountScene : public Scene {
        private:
            sf::Sprite background;
            sf::Text titleText;
            std::vector<std::unique_ptr<Button>> buttons;
            std::unique_ptr<Button> exitButton;

            int selectedCount = 0;

        public:
            PlayerCountScene();

            void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
            void update(float deltaTime, const sf::RenderWindow& window) override;
            void render(sf::RenderWindow& window) override;

            int getSelectedCount() const { return selectedCount; }
            void reset() { selectedCount = 0; }
        };

    } // namespace gui
} // namespace coup