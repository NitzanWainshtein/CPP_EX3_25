// GUI/CoupGUI.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "core/SceneManager.hpp"
#include "core/ResourceManager.hpp"
#include "game/GameController.hpp"

namespace coup {
    namespace gui {

        class CoupGUI {
        private:
            sf::RenderWindow window;
            sf::Clock clock;

            std::unique_ptr<SceneManager> sceneManager;
            std::unique_ptr<GameController> gameController;

            bool running = true;

        public:
            CoupGUI();
            ~CoupGUI() = default;

            bool initialize();
            void run();

        private:
            void handleEvents();
            void update(float deltaTime);
            void render();
            void setupScenes();
        };

    } // namespace gui
} // namespace coup