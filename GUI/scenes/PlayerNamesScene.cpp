// GUI/scenes/PlayerNamesScene.cpp
#include "PlayerNamesScene.hpp"
#include "../core/ResourceManager.hpp"
#include <algorithm>

namespace coup {
namespace gui {

PlayerNamesScene::PlayerNamesScene(GameController* controller)
    : gameController(controller) {

    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    // Title
    titleText.setFont(font);
    titleText.setString("Enter Player Names");
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color::White);

    // Instruction
    instructionText.setFont(font);
    instructionText.setString("Click on a field and type the name. Press TAB to move to next.");
    instructionText.setCharacterSize(18);
    instructionText.setFillColor(sf::Color(200, 200, 200));

    // Buttons
    startButton = std::make_unique<Button>(
        740, 700, 180, 50, font, "Start Game",
        [this]() { tryCreateGame(); }
    );
    startButton->setColors(
        sf::Color(60, 120, 60),
        sf::Color(80, 150, 80),
        sf::Color(40, 90, 40),
        sf::Color(50, 50, 50)
    );

    backButton = std::make_unique<Button>(
        360, 700, 180, 50, font, "Back",
        [this]() {
            reset();
            transitionTo("PlayerCount");
        }
    );

    exitButton = std::make_unique<Button>(
        1180, 20, 80, 40, font, "Exit",
        [this]() {
            reset();
            gameController->resetGame();
            transitionTo("MainMenu");
        }
    );
    exitButton->setColors(
        sf::Color(150, 50, 50),
        sf::Color(180, 70, 70),
        sf::Color(120, 30, 30),
        sf::Color(50, 50, 50)
    );

    // Error popup
    errorPopup = std::make_unique<Popup>(font);

    // Background
    try {
        sf::Texture& bgTex = rm.getTexture("game_bg");
        background.setTexture(bgTex);
        background.setScale(
            1280.0f / bgTex.getSize().x,
            900.0f / bgTex.getSize().y
        );
    } catch (...) {}
}

void PlayerNamesScene::setPlayerCount(int count) {
    playerCount = count;
    names.resize(count);
    activeInput = 0;
    createInputFields();
}

void PlayerNamesScene::createInputFields() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    inputBoxes.clear();
    inputTexts.clear();
    labels.clear();

    float startY = 250;
    float spacing = 80;
    float boxWidth = 300;
    float boxHeight = 50;
    float centerX = 640;

    for (int i = 0; i < playerCount; ++i) {
        float y = startY + i * spacing;

        // Label
        sf::Text label;
        label.setFont(font);
        label.setString("Player " + std::to_string(i + 1) + ":");
        label.setCharacterSize(24);
        label.setFillColor(sf::Color::White);
        label.setPosition(centerX - boxWidth/2 - 120, y + 10);
        labels.push_back(label);

        // Input box
        sf::RectangleShape box;
        box.setSize({boxWidth, boxHeight});
        box.setPosition(centerX - boxWidth/2, y);
        box.setFillColor(sf::Color(60, 60, 60));
        box.setOutlineThickness(2);
        box.setOutlineColor(i == activeInput ? sf::Color::Yellow : sf::Color(100, 100, 100));
        inputBoxes.push_back(box);

        // Input text
        sf::Text text;
        text.setFont(font);
        text.setString(names[i]);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        text.setPosition(centerX - boxWidth/2 + 10, y + 12);
        inputTexts.push_back(text);
    }
}

void PlayerNamesScene::reset() {
    for (auto& name : names) {
        name.clear();
    }
    activeInput = 0;
    if (!inputBoxes.empty()) {
        createInputFields();
    }
}

void PlayerNamesScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    // Handle popup first
    if (errorPopup->isVisible()) {
        errorPopup->handleEvent(event, window);
        return;
    }

    // Handle buttons
    startButton->handleEvent(event, window);
    backButton->handleEvent(event, window);
    exitButton->handleEvent(event, window);

    // Handle input selection
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        for (size_t i = 0; i < inputBoxes.size(); ++i) {
            if (inputBoxes[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                activeInput = i;
                createInputFields(); // Update colors
                break;
            }
        }
    }

    // Handle text input
    if (event.type == sf::Event::TextEntered) {
        handleTextInput(event.text.unicode);
    }

    // Handle keyboard navigation
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tab) {
            activeInput = (activeInput + 1) % playerCount;
            createInputFields();
        } else if (event.key.code == sf::Keyboard::Enter) {
            if (validateNames()) {
                tryCreateGame();
            }
        }
    }
}

void PlayerNamesScene::handleTextInput(sf::Uint32 unicode) {
    if (unicode == '\b') { // Backspace
        if (!names[activeInput].empty()) {
            names[activeInput].pop_back();
            inputTexts[activeInput].setString(names[activeInput]);
        }
    } else if (unicode < 128 && unicode >= 32) { // Printable ASCII
        if (names[activeInput].length() < 20) { // Max name length
            names[activeInput] += static_cast<char>(unicode);
            inputTexts[activeInput].setString(names[activeInput]);
        }
    }
}

bool PlayerNamesScene::validateNames() {
    // Check for empty names
    for (const auto& name : names) {
        if (name.empty()) {
            errorPopup->show("All players must have names!");
            return false;
        }
    }

    // Check for duplicates
    std::vector<std::string> sortedNames = names;
    std::sort(sortedNames.begin(), sortedNames.end());
    if (std::adjacent_find(sortedNames.begin(), sortedNames.end()) != sortedNames.end()) {
        errorPopup->show("Each player must have a unique name!");
        return false;
    }

    return true;
}

void PlayerNamesScene::tryCreateGame() {
    if (!validateNames()) {
        return;
    }

    if (gameController->createGame(names)) {
        transitionTo("Game");
    } else {
        errorPopup->show("Failed to create game! Please try again.",
            [this]() { reset(); });
    }
}

void PlayerNamesScene::update(float deltaTime, const sf::RenderWindow& window) {
    startButton->update(window);
    backButton->update(window);
    exitButton->update(window);
    errorPopup->update(window);
}

void PlayerNamesScene::render(sf::RenderWindow& window) {
    window.clear(sf::Color(30, 30, 40));

    // Background
    window.draw(background);

    // Title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition(640 - titleBounds.width/2, 100);
    window.draw(titleText);

    // Instructions
    sf::FloatRect instrBounds = instructionText.getLocalBounds();
    instructionText.setPosition(640 - instrBounds.width/2, 160);
    window.draw(instructionText);

    // Input fields
    for (size_t i = 0; i < inputBoxes.size(); ++i) {
        window.draw(labels[i]);
        window.draw(inputBoxes[i]);
        window.draw(inputTexts[i]);
    }

    // Buttons
    startButton->draw(window);
    backButton->draw(window);
    exitButton->draw(window);

    // Popup (if visible)
    errorPopup->draw(window);
}

} // namespace gui
} // namespace coup