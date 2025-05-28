// GUI/scenes/PlayerCountScene.cpp
#include "PlayerCountScene.hpp"
#include "../core/ResourceManager.hpp"

namespace coup {
namespace gui {

PlayerCountScene::PlayerCountScene() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    // Title
    titleText.setFont(font);
    titleText.setString("Select Number of Players");
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color::White);

    // Center title
    sf::FloatRect bounds = titleText.getLocalBounds();
    titleText.setPosition(640 - bounds.width/2, 150);

    // Create number buttons (2-6)
    float startX = 340;
    float y = 300;
    float spacing = 120;

    for (int i = 2; i <= 6; ++i) {
        auto btn = std::make_unique<Button>(
            startX + (i-2) * spacing, y, 100, 100, font, std::to_string(i),
            [this, i]() {
                selectedCount = i;
                transitionTo("PlayerNames");
            }
        );
        btn->setColors(
            sf::Color(70, 70, 120),    // normal - blue
            sf::Color(90, 90, 150),    // hover
            sf::Color(50, 50, 90),     // pressed
            sf::Color(50, 50, 50)      // disabled
        );
        buttons.push_back(std::move(btn));
    }

    // Exit button (top-right)
    exitButton = std::make_unique<Button>(
        1180, 20, 80, 40, font, "Exit",
        [this]() { transitionTo("MainMenu"); }
    );
    exitButton->setColors(
        sf::Color(150, 50, 50),    // normal - red
        sf::Color(180, 70, 70),    // hover
        sf::Color(120, 30, 30),    // pressed
        sf::Color(50, 50, 50)      // disabled
    );

    // Load background
    try {
        sf::Texture& bgTex = rm.getTexture("game_bg");
        background.setTexture(bgTex);
        background.setScale(
            1280.0f / bgTex.getSize().x,
            900.0f / bgTex.getSize().y
        );
    } catch (...) {
        // No background texture
    }
}

void PlayerCountScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    exitButton->handleEvent(event, window);

    for (auto& button : buttons) {
        button->handleEvent(event, window);
    }
}

void PlayerCountScene::update(float deltaTime, const sf::RenderWindow& window) {
    exitButton->update(window);

    for (auto& button : buttons) {
        button->update(window);
    }
}

void PlayerCountScene::render(sf::RenderWindow& window) {
    window.clear(sf::Color(30, 30, 40));

    // Background
    window.draw(background);

    // Title
    window.draw(titleText);

    // Number buttons
    for (auto& button : buttons) {
        button->draw(window);
    }

    // Exit button
    exitButton->draw(window);
}

} // namespace gui
} // namespace coup