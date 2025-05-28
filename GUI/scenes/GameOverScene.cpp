// GUI/scenes/GameOverScene.cpp
#include "GameOverScene.hpp"
#include "../core/ResourceManager.hpp"

namespace coup {
namespace gui {

GameOverScene::GameOverScene(GameController* controller)
    : gameController(controller) {

    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    // Background
    try {
        sf::Texture& bgTex = rm.getTexture("game_bg");
        background.setTexture(bgTex);
        background.setScale(
            1280.0f / bgTex.getSize().x,
            900.0f / bgTex.getSize().y
        );
    } catch (...) {}

    // Game Over text
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(72);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);

    // Winner text
    winnerText.setFont(font);
    winnerText.setCharacterSize(48);
    winnerText.setFillColor(sf::Color::Yellow);
    winnerText.setStyle(sf::Text::Bold);

    // Stats title
    statsTitle.setFont(font);
    statsTitle.setString("Final Statistics:");
    statsTitle.setCharacterSize(28);
    statsTitle.setFillColor(sf::Color::White);

    // Buttons
    playAgainButton = std::make_unique<Button>(
        440, 700, 200, 60, font, "Play Again",
        [this]() {
            gameController->resetGame();
            transitionTo("PlayerCount");
        }
    );
    playAgainButton->setColors(
        sf::Color(60, 120, 60),
        sf::Color(80, 150, 80),
        sf::Color(40, 90, 40),
        sf::Color(50, 50, 50)
    );

    mainMenuButton = std::make_unique<Button>(
        640, 700, 200, 60, font, "Main Menu",
        [this]() {
            gameController->resetGame();
            transitionTo("MainMenu");
        }
    );
}

void GameOverScene::onEnter() {
    animationTimer = 0.0f;

    // Update winner text
    std::string winner = gameController->getWinner();
    if (!winner.empty()) {
        winnerText.setString("Winner: " + winner + "!");

        // Get winner's role
        Player* winnerPlayer = gameController->getPlayer(winner);
        if (winnerPlayer) {
            winnerText.setString("Winner: " + winner + " (" + winnerPlayer->getRoleName() + ")!");
        }
    }

    // Center texts
    sf::FloatRect gameOverBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(640 - gameOverBounds.width/2, 100);

    sf::FloatRect winnerBounds = winnerText.getLocalBounds();
    winnerText.setPosition(640 - winnerBounds.width/2, 200);

    sf::FloatRect statsBounds = statsTitle.getLocalBounds();
    statsTitle.setPosition(640 - statsBounds.width/2, 320);

    createPlayerStats();
}

void GameOverScene::createPlayerStats() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    playerStats.clear();

    auto playerNames = gameController->getPlayerNames();
    float startY = 380;
    float spacing = 35;

    for (size_t i = 0; i < playerNames.size(); ++i) {
        const std::string& name = playerNames[i];
        Player* player = gameController->getPlayer(name);

        if (player) {
            sf::Text stat;
            stat.setFont(font);
            stat.setCharacterSize(20);
            stat.setPosition(400, startY + i * spacing);

            std::string status = gameController->isPlayerAlive(name) ? "ALIVE" : "ELIMINATED";
            std::string text = name + " (" + player->getRoleName() + ") - " +
                             std::to_string(player->getCoins()) + " coins - " + status;

            stat.setString(text);

            // Color based on status
            if (name == gameController->getWinner()) {
                stat.setFillColor(sf::Color::Yellow);
                stat.setStyle(sf::Text::Bold);
            } else if (gameController->isPlayerAlive(name)) {
                stat.setFillColor(sf::Color::Green);
            } else {
                stat.setFillColor(sf::Color(150, 150, 150));
            }

            playerStats.push_back(stat);
        }
    }
}

void GameOverScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    playAgainButton->handleEvent(event, window);
    mainMenuButton->handleEvent(event, window);
}

void GameOverScene::update(float deltaTime, const sf::RenderWindow& window) {
    animationTimer += deltaTime;

    playAgainButton->update(window);
    mainMenuButton->update(window);

    // Animate winner text
    float scale = 1.0f + 0.1f * sin(animationTimer * 3);
    winnerText.setScale(scale, scale);

    // Re-center after scaling
    sf::FloatRect winnerBounds = winnerText.getLocalBounds();
    winnerText.setPosition(640 - (winnerBounds.width * scale)/2, 200);
}

void GameOverScene::render(sf::RenderWindow& window) {
    window.clear(sf::Color(30, 30, 40));

    // Background
    window.draw(background);

    // Texts
    window.draw(gameOverText);
    window.draw(winnerText);
    window.draw(statsTitle);

    // Player stats
    for (const auto& stat : playerStats) {
        window.draw(stat);
    }

    // Buttons
    playAgainButton->draw(window);
    mainMenuButton->draw(window);
}

} // namespace gui
} // namespace coup