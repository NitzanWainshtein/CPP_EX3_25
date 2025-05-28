// GUI/scenes/MainMenuScene.cpp
#include "MainMenuScene.hpp"
#include "../core/ResourceManager.hpp"

namespace coup {
namespace gui {

MainMenuScene::MainMenuScene() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    // Title
    titleText.setFont(font);
    titleText.setString("COUP");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);

    // Center title
    sf::FloatRect bounds = titleText.getLocalBounds();
    titleText.setPosition(640 - bounds.width/2, 200);

    // Credits
    creditsText.setFont(font);
    creditsText.setString("Made by Nitzan Wainshtein");
    creditsText.setCharacterSize(18);
    creditsText.setFillColor(sf::Color(180, 180, 180));

    // Center credits at bottom
    sf::FloatRect creditsBounds = creditsText.getLocalBounds();
    creditsText.setPosition(640 - creditsBounds.width/2, 850);

    // Start Game button
    auto startBtn = std::make_unique<Button>(
        540, 400, 200, 60, font, "Start Game",
        [this]() { transitionTo("PlayerCount"); }
    );
    startBtn->setColors(
        sf::Color(60, 120, 60),    // normal - green
        sf::Color(80, 150, 80),    // hover
        sf::Color(40, 90, 40),     // pressed
        sf::Color(50, 50, 50)      // disabled
    );
    buttons.push_back(std::move(startBtn));

    // Exit Game button
    auto exitBtn = std::make_unique<Button>(
        540, 480, 200, 60, font, "Exit Game",
        [this]() { transitionTo("Exit"); }
    );
    exitBtn->setColors(
        sf::Color(120, 60, 60),    // normal - red
        sf::Color(150, 80, 80),    // hover
        sf::Color(90, 40, 40),     // pressed
        sf::Color(50, 50, 50)      // disabled
    );
    buttons.push_back(std::move(exitBtn));

    // Load background
    try {
        sf::Texture& bgTex = rm.getTexture("menu_bg");
        background.setTexture(bgTex);
        // Scale to fit window
        background.setScale(
            1280.0f / bgTex.getSize().x,
            900.0f / bgTex.getSize().y
        );
    } catch (...) {
        // No background texture - that's OK
    }
}

void MainMenuScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    for (auto& button : buttons) {
        button->handleEvent(event, window);
    }
}

void MainMenuScene::update(float /*deltaTime*/, const sf::RenderWindow& window) {
    for (auto& button : buttons) {
        button->update(window);
    }
}

void MainMenuScene::render(sf::RenderWindow& window) {
    window.clear(sf::Color(30, 30, 40));

    // Draw background if available
    window.draw(background);

    // Draw title
    window.draw(titleText);

    // Draw buttons
    for (auto& button : buttons) {
        button->draw(window);
    }

    // Draw credits
    window.draw(creditsText);
}

} // namespace gui
} // namespace coup