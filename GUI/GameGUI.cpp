// Email: nitzanwa@gmail.com

#include "GameGUI.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <stdexcept>
#include "../GameLogic/Game.hpp"
#include "../GameLogic/BankManager.hpp"
#include "../GameLogic/PlayerFactory.hpp"
#include "../Players/Roles/Governor.hpp"
#include "../Players/Roles/Spy.hpp"
#include "../Players/Roles/Baron.hpp"
#include "../Players/Roles/General.hpp"
#include "../Players/Roles/Judge.hpp"
#include "../Players/Roles/Merchant.hpp"

namespace coup {

// ------------------- Button Implementation -------------------

Button::Button(float x, float y, float width, float height, const sf::Font &font, const std::string &label,
               std::function<void()> cb)
    : onClick(std::move(cb)) {
    shape.setPosition(x, y);
    shape.setSize({width, height});
    shape.setFillColor(sf::Color(90, 90, 90));
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color(120, 120, 120));

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(x + (width - textBounds.width) / 2 - textBounds.left,
                     y + (height - textBounds.height) / 2 - textBounds.top);
}

void Button::draw(sf::RenderWindow &window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::isClicked(sf::Vector2i mousePos) {
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Button::setEnabled(bool enabled) {
    if (enabled) {
        shape.setFillColor(sf::Color(90, 90, 90));
        text.setFillColor(sf::Color::White);
    } else {
        shape.setFillColor(sf::Color(60, 60, 60));
        text.setFillColor(sf::Color(100, 100, 100));
    }
}

// ------------------- MessagePopup Implementation -------------------

MessagePopup::MessagePopup(const sf::Font &font) : isVisible(false) {
    // Semi-transparent overlay
    overlay.setSize(sf::Vector2f(1280, 900));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    // Popup box - made bigger for longer messages
    popup.setSize(sf::Vector2f(600, 250));
    popup.setPosition(340, 325);
    popup.setFillColor(sf::Color(40, 40, 50));
    popup.setOutlineThickness(3);
    popup.setOutlineColor(sf::Color::Red);

    // Message text - with word wrapping
    messageText.setFont(font);
    messageText.setCharacterSize(18);
    messageText.setFillColor(sf::Color::White);
    messageText.setPosition(360, 360);

    // OK button
    button.setSize(sf::Vector2f(100, 40));
    button.setPosition(590, 520);
    button.setFillColor(sf::Color(80, 80, 80));
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::White);

    buttonText.setFont(font);
    buttonText.setString("OK");
    buttonText.setCharacterSize(18);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setPosition(625, 530);
}

void MessagePopup::show(const std::string &message) {
    currentMessage = message;

    // Simple word wrapping - break long lines
    std::string wrappedMessage = message;
    size_t maxLineLength = 50; // Characters per line

    for (size_t i = maxLineLength; i < wrappedMessage.length(); i += maxLineLength + 1) {
        size_t spacePos = wrappedMessage.find(' ', i);
        if (spacePos != std::string::npos && spacePos < i + 15) {
            wrappedMessage[spacePos] = '\n';
            i = spacePos;
        }
    }

    messageText.setString(wrappedMessage);
    isVisible = true;
}

void MessagePopup::hide() {
    isVisible = false;
}

bool MessagePopup::handleClick(sf::Vector2i mousePos) {
    if (!isVisible) return false;

    if (button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
        hide();
        return true;
    }
    return false;
}

void MessagePopup::draw(sf::RenderWindow &window) {
    if (!isVisible) return;

    window.draw(overlay);
    window.draw(popup);
    window.draw(messageText);
    window.draw(button);
    window.draw(buttonText);
}

// ------------------- CoupGUI Implementation -------------------

CoupGUI::CoupGUI() : window(sf::VideoMode(1280, 900), "Coup Game GUI", sf::Style::Titlebar | sf::Style::Close),
                     currentState(State::Start) {
    window.setFramerateLimit(60);

    // Try multiple font paths
    bool fontLoaded = false;
    std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/Windows/Fonts/arial.ttf",
        "arial.ttf",
        "DejaVuSans-Bold.ttf"
    };

    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            std::cout << "Font loaded from: " << path << std::endl;
            break;
        }
    }

    if (!fontLoaded) {
        std::cerr << "Warning: Could not load any font, using default" << std::endl;
        // SFML will use default font if no font is loaded
    }

    popup = MessagePopup(font);

    // Load background images (optional)
    loadBackgrounds();

    // Initialize message text
    messageText.setFont(font);
    messageText.setCharacterSize(18);
    messageText.setFillColor(sf::Color::White);
    messageText.setPosition(50, 820);

    // Initialize flags
    turnInProgress = false;
    playerCountSelected = false;
    messageTimer = 0.0f;
    waitingForBribeDecision = false;
}

CoupGUI::~CoupGUI() {
    // Clean up players if they exist
    for (Player *p : players) {
        delete p;
    }
    players.clear();
}

void CoupGUI::loadBackgrounds() {
    // Try to load menu background
    if (!menuBackgroundTexture.loadFromFile("GUI/images/background_menu.png")) {
        std::cerr << "Warning: Could not load menu background image" << std::endl;
    }

    // Try to load game background
    if (!gameBackgroundTexture.loadFromFile("GUI/images/game_background.png")) {
        std::cerr << "Warning: Could not load game background image" << std::endl;
    }
}

void CoupGUI::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            handleEvent(event);
        }

        float deltaTime = clock.restart().asSeconds();
        render(deltaTime);
    }
}

void CoupGUI::handleEvent(const sf::Event &event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        // Handle popup first
        if (popup.visible()) {
            popup.handleClick(sf::Mouse::getPosition(window));
            return;
        }

        // Handle regular buttons
        for (auto &btn : buttons) {
            if (btn.isClicked(sf::Mouse::getPosition(window))) {
                btn.onClick();
                break;
            }
        }
    } else if (currentState == State::EnterNames && event.type == sf::Event::TextEntered) {
        if (popup.visible()) return; // Don't handle text input when popup is shown

        if (event.text.unicode == '\b' && !nameBuffers[nameIndex].empty()) {
            nameBuffers[nameIndex].pop_back();
        } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
            if (!nameBuffers[nameIndex].empty() && nameIndex < playerCount - 1) {
                nameIndex++;
            }
        } else if (event.text.unicode < 128 && event.text.unicode >= 32) {
            nameBuffers[nameIndex] += static_cast<char>(event.text.unicode);
        }
    }
}

void CoupGUI::render(float deltaTime) {
    window.clear(sf::Color(30, 30, 40));

    // Draw background based on state
    if (currentState == State::Start) {
        if (menuBackgroundTexture.getSize().x > 0) {
            backgroundSprite.setTexture(menuBackgroundTexture);
            backgroundSprite.setScale(
                (float)window.getSize().x / menuBackgroundTexture.getSize().x,
                (float)window.getSize().y / menuBackgroundTexture.getSize().y
            );
            window.draw(backgroundSprite);
        }
    } else {
        if (gameBackgroundTexture.getSize().x > 0) {
            backgroundSprite.setTexture(gameBackgroundTexture);
            backgroundSprite.setScale(
                (float)window.getSize().x / gameBackgroundTexture.getSize().x,
                (float)window.getSize().y / gameBackgroundTexture.getSize().y
            );
            window.draw(backgroundSprite);
        }
    }

    // Draw content based on state
    if (currentState == State::Start)
        drawStartMenu();
    else if (currentState == State::SelectCount)
        drawPlayerCountButtons();
    else if (currentState == State::EnterNames)
        drawNameInputs();
    else if (currentState == State::Playing || currentState == State::GameOver) {
        drawPlayers();
        if (currentState == State::Playing) {
            drawPlayerStatus();
            // Draw special abilities for current player
            Player* current = getCurrentPlayer();
            if (current && turnInProgress) {
                std::vector<Player*> validTargets = getValidTargets(current);
                drawSpecialAbilities(current, validTargets);
            }
        }
    }

    // Always draw exit button (except on start screen)
    if (currentState != State::Start) {
        drawExitButton();
    }

    // Draw all buttons (if popup not visible)
    if (!popup.visible()) {
        for (auto &b : buttons)
            b.draw(window);
    }

    // Message handling
    if (messageTimer > 0 && !popup.visible()) {
        messageTimer -= deltaTime;
        window.draw(messageText);
    }

    // Draw popup last (on top of everything)
    popup.draw(window);

    window.display();
}

void CoupGUI::drawExitButton() {
    // Small exit button in top-right corner
    Button exitBtn(1180, 10, 80, 30, font, "Exit", [this]() {
        resetGame();
        currentState = State::Start;
    });
    exitBtn.draw(window);

    // Check if clicked (simple manual check since it's not in buttons vector)
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        if (exitBtn.isClicked(sf::Mouse::getPosition(window))) {
            resetGame();
            currentState = State::Start;
        }
    }
}

void CoupGUI::drawStartMenu() {
    buttons.clear();

    // Credits at bottom as requested
    sf::Text credits;
    credits.setFont(font);
    credits.setString("Made by Nitzan Wainshtein");
    credits.setCharacterSize(18);
    credits.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect creditsBounds = credits.getLocalBounds();
    credits.setPosition((window.getSize().x - creditsBounds.width) / 2, 850);
    window.draw(credits);

    // Just the buttons, centered (removed "COUP GAME" title)
    buttons.emplace_back(540, 400, 200, 60, font, "Start Game", [this]() {
        currentState = State::SelectCount;
        playerCountSelected = false;
    });

    buttons.emplace_back(540, 480, 200, 60, font, "Exit Game", [this]() {
        window.close();
    });
}

void CoupGUI::drawPlayerCountButtons() {
    buttons.clear();

    sf::Text prompt;
    prompt.setFont(font);
    prompt.setString("Select number of players:");
    prompt.setCharacterSize(32);
    prompt.setFillColor(sf::Color::White);
    sf::FloatRect promptBounds = prompt.getLocalBounds();
    prompt.setPosition((window.getSize().x - promptBounds.width) / 2, 150);
    window.draw(prompt);

    // Only show count buttons if not selected yet
    if (!playerCountSelected) {
        for (int i = 2; i <= 6; ++i) {
            buttons.emplace_back(450 + (i - 2) * 80, 300, 60, 60, font, std::to_string(i), [this, i]() {
                playerCount = i;
                nameBuffers = std::vector<std::string>(playerCount);
                playerCountSelected = true;
                nameIndex = 0;
            });
        }
    } else {
        // Show selected count
        sf::Text selectedText;
        selectedText.setFont(font);
        selectedText.setString("Selected: " + std::to_string(playerCount) + " players");
        selectedText.setCharacterSize(24);
        selectedText.setFillColor(sf::Color::Green);
        sf::FloatRect selectedBounds = selectedText.getLocalBounds();
        selectedText.setPosition((window.getSize().x - selectedBounds.width) / 2, 320);
        window.draw(selectedText);

        buttons.emplace_back(540, 380, 200, 50, font, "Continue", [this]() {
            currentState = State::EnterNames;
        });
    }
}

void CoupGUI::drawNameInputs() {
    buttons.clear();

    sf::Text prompt;
    prompt.setFont(font);
    prompt.setString("Enter player names:");
    prompt.setCharacterSize(28);
    prompt.setFillColor(sf::Color::White);
    sf::FloatRect promptBounds = prompt.getLocalBounds();
    prompt.setPosition((window.getSize().x - promptBounds.width) / 2, 80);
    window.draw(prompt);

    sf::Text instruction;
    instruction.setFont(font);
    instruction.setString("Type name and press Enter to move to next player");
    instruction.setCharacterSize(18);
    instruction.setFillColor(sf::Color(180, 180, 180));
    sf::FloatRect instrBounds = instruction.getLocalBounds();
    instruction.setPosition((window.getSize().x - instrBounds.width) / 2, 120);
    window.draw(instruction);

    // Display all name inputs
    for (int i = 0; i < playerCount; ++i) {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(24);
        text.setFillColor(i == nameIndex
                          ? sf::Color::Green
                          : (!nameBuffers[i].empty() ? sf::Color::White : sf::Color(120, 120, 120)));

        float yPos = 180 + i * 50;
        text.setPosition(400, yPos);

        std::string displayText = "Player " + std::to_string(i + 1) + ": " + nameBuffers[i];
        if (i == nameIndex) displayText += "_";

        text.setString(displayText);
        window.draw(text);
    }

    // Check if all names are filled
    bool allNamesFilled = true;
    for (const auto &name : nameBuffers) {
        if (name.empty()) {
            allNamesFilled = false;
            break;
        }
    }

    // Show start game button if all names are filled
    if (allNamesFilled) {
        buttons.emplace_back(540, 180 + playerCount * 50 + 30, 200, 50, font, "Start Game", [this]() {
            // Check for duplicate names BEFORE creating the game
            for (size_t i = 0; i < nameBuffers.size(); ++i) {
                for (size_t j = i + 1; j < nameBuffers.size(); ++j) {
                    if (nameBuffers[i] == nameBuffers[j]) {
                        showPopup(
                            "Error: Player name '" + nameBuffers[i] +
                            "' is used more than once! Please choose unique names.");
                        return;
                    }
                }
            }

            playerNames = nameBuffers;
            startGame();
        });
    }

    // Back button to change player count
    buttons.emplace_back(340, 180 + playerCount * 50 + 30, 150, 50, font, "Back", [this]() {
        currentState = State::SelectCount;
        playerCountSelected = false;
        nameBuffers.clear();
        nameIndex = 0;
    });
}

bool CoupGUI::isDuplicateName(const std::string &name) const {
    int count = 0;
    for (const auto &bufferName : nameBuffers) {
        if (bufferName == name) {
            count++;
            if (count > 1) return true;
        }
    }
    return false;
}

void CoupGUI::startGame() {
    try {
        // Reset any existing game
        resetGame();

        // Create a new game
        game = std::make_unique<Game>();

        // Create players with random roles
        players.clear();
        for (const auto &name : playerNames) {
            players.push_back(randomPlayer(*game, name));
        }

        // Reset turn state
        turnInProgress = false;
        waitingForBribeDecision = false;

        setupCallbacks();
        currentState = State::Playing;
        updateButtons();

        showMessage("Game started! Click 'Start Turn' when " + game->turn() + " is ready.");
    } catch (const std::exception &e) {
        showPopup("Failed to start game: " + std::string(e.what()));
        resetGame();
        currentState = State::EnterNames;
        nameIndex = 0;
    }
}

void CoupGUI::resetGame() {
    // Clean up existing players
    for (Player *p : players) {
        delete p;
    }
    players.clear();

    // Reset game state
    game.reset();
    turnInProgress = false;
    waitingForBribeDecision = false;
    messageTimer = 0.0f;
}

void CoupGUI::setupCallbacks() {
    for (auto *p : players) {
        // Set up bribe callback to intercept the decision
        p->setBribeDecisionCallback([this, p](const Player &) -> bool {
            // If it's the current player's turn and they can bribe,
            // return true to prevent automatic endTurn, then set the flag
            Player *current = getCurrentPlayer();
            if (current == p && p->canUseBribe()) {
                waitingForBribeDecision = true;
                return true; // This prevents endTurn() from being called
            }
            return false;
        });

        p->setBlockDecisionCallback([this, p](const Player &blocker, ActionType action, const Player *actor) -> bool {
            std::string actionName;
            switch (action) {
                case ActionType::Tax:
                    actionName = "tax";
                    break;
                case ActionType::Bribe:
                    actionName = "bribe";
                    break;
                case ActionType::Coup:
                    actionName = "coup";
                    break;
                default:
                    actionName = "action";
                    break;
            }

            bool canBlock = false;
            if (blocker.getRoleName() == "Governor" && action == ActionType::Tax) {
                canBlock = true;
            } else if (blocker.getRoleName() == "Judge" && action == ActionType::Bribe) {
                canBlock = true;
            } else if (blocker.getRoleName() == "General" && action == ActionType::Coup && blocker.getCoins() >= 5) {
                canBlock = true;
            }

            if (canBlock) {
                showMessage(blocker.getName() + " blocks " + actor->getName() + "'s " + actionName + "!");
                return true;
            }
            return false;
        });
    }
}

Player* CoupGUI::getCurrentPlayer() {
    // Check if game is over FIRST
    if (currentState == State::GameOver) return nullptr;

    try {
        // Double check game state
        if (game->isGameOver()) {
            return nullptr;
        }

        std::string currentName = game->turn();
        for (auto *p : players) {
            if (p != nullptr && game->isAlive(*p) && p->getName() == currentName) {
                return p;
            }
        }
    } catch (const std::exception &e) {
        // Game over or error
        return nullptr;
    }
    return nullptr;
}

void CoupGUI::drawPlayers() {
    if (!game) return;

    // Game info
    sf::Text gameInfo;
    gameInfo.setFont(font);
    gameInfo.setString("Bank: " + std::to_string(game->getBank()) + " coins");
    gameInfo.setCharacterSize(24);
    gameInfo.setFillColor(sf::Color::White);
    gameInfo.setPosition(40, 40);
    window.draw(gameInfo);

    // Current turn info
    sf::Text turnInfo;
    turnInfo.setFont(font);
    turnInfo.setCharacterSize(22);
    turnInfo.setPosition(40, 70);
    try {
        if (game->isGameOver()) {
            turnInfo.setString("Game Over");
            turnInfo.setFillColor(sf::Color::Red);
        } else {
            std::string currentTurn = game->turn();
            turnInfo.setString("Current Turn: " + currentTurn);
            turnInfo.setFillColor(sf::Color::Yellow);
        }
    } catch (...) {
        turnInfo.setString("Game Over");
        turnInfo.setFillColor(sf::Color::Red);
    }
    window.draw(turnInfo);

    // Turn status
    sf::Text turnStatus;
    turnStatus.setFont(font);
    turnStatus.setCharacterSize(18);
    turnStatus.setPosition(40, 100);

    Player *current = getCurrentPlayer();
    if (current) {
        if (waitingForBribeDecision) {
            turnStatus.setString("Choose: Bribe for bonus action or End Turn");
            turnStatus.setFillColor(sf::Color::Green);
        } else if (!turnInProgress) {
            if (current->getCoins() >= 10) {
                turnStatus.setString("YOU MUST COUP! (10+ coins)");
                turnStatus.setFillColor(sf::Color::Red);
            } else {
                turnStatus.setString("Click 'Start Turn' to begin");
                turnStatus.setFillColor(sf::Color::White);
            }
        } else {
            if (current->getLastAction() == ActionType::None) {
                turnStatus.setString("Choose your action");
                turnStatus.setFillColor(sf::Color::White);
            } else {
                turnStatus.setString("Click 'End Turn' to finish");
                turnStatus.setFillColor(sf::Color::Yellow);
            }
        }
    } else {
        turnStatus.setString("Game Over");
        turnStatus.setFillColor(sf::Color::Red);
    }
    window.draw(turnStatus);

    // Players list
    sf::Text playersTitle;
    playersTitle.setFont(font);
    playersTitle.setString("Players:");
    playersTitle.setCharacterSize(20);
    playersTitle.setFillColor(sf::Color::White);
    playersTitle.setPosition(40, 140);
    window.draw(playersTitle);

    // Only display ALIVE players
    int displayIndex = 0;
    for (size_t i = 0; i < players.size(); ++i) {
        Player *p = players[i];
        if (p == nullptr || !game->isAlive(*p)) continue;

        bool isCurrentTurn = false;
        try {
            if (!game->isGameOver()) {
                isCurrentTurn = (game->turn() == p->getName());
            }
        } catch (...) {
            isCurrentTurn = false;
        }

        float yPos = 170.f + displayIndex * 35;

        // Highlight current player
        if (isCurrentTurn) {
            sf::RectangleShape highlight;
            highlight.setPosition(35, yPos - 3);
            highlight.setSize(sf::Vector2f(400, 30));
            highlight.setFillColor(sf::Color(60, 120, 60, 100));
            window.draw(highlight);
        }

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(18);
        text.setPosition(40.f, yPos);
        text.setFillColor(isCurrentTurn ? sf::Color::Yellow : sf::Color::White);

        std::string statusText = "";
        if (p->isSanctioned()) statusText += " [SANCTIONED]";

        text.setString(p->getName() + " (" + p->getRoleName() + ") - " +
                      std::to_string(p->getCoins()) + " coins" + statusText);
        window.draw(text);

        displayIndex++;
    }

    // Game over state
    if (currentState == State::GameOver) {
        sf::RectangleShape overlay;
        overlay.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        sf::Text winnerText;
        winnerText.setFont(font);
        winnerText.setCharacterSize(48);
        winnerText.setFillColor(sf::Color::Green);
        winnerText.setStyle(sf::Text::Bold);

        try {
            std::string winnerName = game->winner();
            winnerText.setString("Winner: " + winnerName + "!");
            sf::FloatRect winnerBounds = winnerText.getLocalBounds();
            winnerText.setPosition((window.getSize().x - winnerBounds.width) / 2, 300);
            window.draw(winnerText);
        } catch (const std::exception &e) {
            winnerText.setString("Game Over!");
            sf::FloatRect winnerBounds = winnerText.getLocalBounds();
            winnerText.setPosition((window.getSize().x - winnerBounds.width) / 2, 300);
            window.draw(winnerText);
        }
    }
}

void CoupGUI::showMessage(const std::string &msg) {
    std::cout << "[INFO] " << msg << std::endl;
    currentMessage = msg;
    messageText.setString(msg);
    messageText.setFillColor(sf::Color::White);
    messageTimer = MESSAGE_DURATION;
}

void CoupGUI::showError(const std::string &msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
    currentMessage = "ERROR: " + msg;
    messageText.setString(currentMessage);
    messageText.setFillColor(sf::Color::Red);
    messageTimer = MESSAGE_DURATION;
}

void CoupGUI::showPopup(const std::string &msg) {
    popup.show(msg);
}

void CoupGUI::checkForWinner() {
    if (!game) return;

    try {
        if (game->isGameOver()) {
            // Prevent multiple calls
            if (currentState == State::GameOver) return;

            currentState = State::GameOver;
            turnInProgress = false;
            waitingForBribeDecision = false;

            // Clear buttons immediately to prevent further actions
            buttons.clear();

            // Show winner message safely
            try {
                std::string winnerName = game->winner();
                showMessage("Game Over! Winner: " + winnerName);
                std::cout << "[INFO] Game Over! Winner: " << winnerName << std::endl;
            } catch (...) {
                showMessage("Game Over!");
                std::cout << "[INFO] Game Over!" << std::endl;
            }

            // Force a render update to show the game over state
            // Don't call updateButtons() here to avoid recursion
        }
    } catch (const std::exception &e) {
        // Game not over yet, or some other issue
        std::cout << "[DEBUG] checkForWinner exception: " << e.what() << std::endl;
    }
}

// Safe target collection
std::vector<Player*> CoupGUI::getValidTargets(Player *current) {
    std::vector<Player*> validTargets;
    if (!current || !game) return validTargets;

    for (auto *p : players) {
        if (p != nullptr && p != current && game->isAlive(*p)) {
            validTargets.push_back(p);
        }
    }
    return validTargets;
}

void CoupGUI::updateButtons() {
    buttons.clear();

    if (currentState == State::GameOver) {
        // Only show Play Again button in game over state
        buttons.emplace_back(540, 400, 200, 60, font, "Play Again", [this]() {
            resetGame();
            currentState = State::Start;
        });
        return;
    }

    if (currentState != State::Playing) return;

    Player *current = getCurrentPlayer();
    if (!current) {
        // No current player - check for game over but don't recursively call updateButtons
        if (game && game->isGameOver() && currentState != State::GameOver) {
            checkForWinner();
        }
        return;
    }
        return;
    }

    if (currentState != State::Playing) return;

    Player *current = getCurrentPlayer();
    if (!current) {
        // No current player - game might be over
        checkForWinner();
        return;
    }

    // Actions area
    float actionsY = 450;
    float actionsX = 50;
    float width = 150;
    float height = 35;
    float xSpacing = 160;
    float ySpacing = 45;
    int buttonsPerRow = 7;
    int buttonCount = 0;

    auto addButton = [&](const std::string &label, std::function<void()> cb) {
        buttons.emplace_back(actionsX, actionsY, width, height, font, label, cb);
        buttonCount++;

        actionsX += xSpacing;
        if (buttonCount % buttonsPerRow == 0) {
            actionsX = 50;
            actionsY += ySpacing;
        }
    };

    // SPECIAL STATE: Waiting for bribe decision
    if (waitingForBribeDecision) {
        if (current->canUseBribe()) {
            addButton("Bribe (4 coins)", [=]() {
                try {
                    current->bribe();
                    if (current->getLastAction() != ActionType::Bribe) {
                        // Bribe was blocked, turn is over
                        showMessage(current->getName() + "'s bribe was blocked! Turn ended.");
                        turnInProgress = false;
                        waitingForBribeDecision = false;
                        checkForWinner();
                        if (currentState != State::GameOver) {
                            try {
                                showMessage("Now it's " + game->turn() + "'s turn.");
                            } catch (...) {
                                checkForWinner();
                            }
                        }
                    } else {
                        // Bribe succeeded, player can take bonus action
                        showMessage(current->getName() + " used bribe for bonus action. Choose your bonus action!");
                        waitingForBribeDecision = false;
                        // DON'T end turn here - let player take bonus action
                    }

                    updateButtons();
                } catch (const std::exception &e) {
                    showPopup("Error: " + std::string(e.what()));
                    waitingForBribeDecision = false;
                    turnInProgress = false;
                    updateButtons();
                }
            });
        }

        addButton("End Turn", [=]() {
            waitingForBribeDecision = false;
            turnInProgress = false;

            // Manually end the turn
            current->endTurn();

            checkForWinner();
            if (currentState != State::GameOver) {
                try {
                    showMessage("Turn ended. Now it's " + game->turn() + "'s turn.");
                } catch (...) {
                    checkForWinner();
                }
            }
            updateButtons();
        });

        return; // Only show bribe decision buttons
    }

    // FORCED COUP with 10+ coins
    if (current->getCoins() >= 10) {
        std::vector<Player *> validTargets = getValidTargets(current);

        for (auto *target : validTargets) {
            std::string targetName = target->getName();
            addButton("Coup " + targetName, [=]() {
                try {
                    // Find target safely
                    Player *safeTarget = nullptr;
                    for (auto *p : players) {
                        if (p != nullptr && game->isAlive(*p) && p->getName() == targetName) {
                            safeTarget = p;
                            break;
                        }
                    }

                    if (safeTarget) {
                        // For forced coup, we manually set the turn state
                        current->resetTurnState();
                        current->coup(*safeTarget);

                        turnInProgress = false;
                        waitingForBribeDecision = false;
                        showMessage(current->getName() + " was forced to coup " + targetName);

                        // Check for winner BEFORE updating buttons
                        std::cout << "[DEBUG] Forced coup completed, checking for winner..." << std::endl;
                        checkForWinner();

                        // Only update buttons if game is not over
                        if (currentState != State::GameOver) {
                            updateButtons();
                        } else {
                            std::cout << "[DEBUG] Game over after forced coup" << std::endl;
                        }
                    } else {
                        showPopup("Error: Target player no longer exists");
                    }
                } catch (const std::exception &e) {
                    showPopup("Error: " + std::string(e.what()));
                    turnInProgress = false;
                    waitingForBribeDecision = false;
                }
            });
        }
        return; // Don't show any other buttons
    }

    // NORMAL TURN FLOW

    // Phase 1: Start Turn
    if (!turnInProgress) {
        addButton("Start Turn", [=]() {
            try {
                current->startTurn();
                turnInProgress = true;
                showMessage(current->getName() + " started their turn.");
                updateButtons();
            } catch (const std::exception &e) {
                showPopup("Error: " + std::string(e.what()));
                if (current->getCoins() >= 10) {
                    // Player must coup
                    updateButtons();
                }
            }
        });
        return;
    }

    // Show only End Turn button if action was already taken and not waiting for bribe
    if (current->getLastAction() != ActionType::None && !waitingForBribeDecision) {
        addButton("End Turn", [=]() {
            try {
                current->endTurn();
                turnInProgress = false;
                showMessage(current->getName() + "'s turn ended.");

                checkForWinner();
                if (currentState != State::GameOver) {
                    try {
                        showMessage("Now it's " + game->turn() + "'s turn.");
                    } catch (...) {
                        checkForWinner();
                    }
                    updateButtons();
                }
            } catch (const std::exception &e) {
                showPopup("Error: " + std::string(e.what()));
            }
        });
        return; // Only show End Turn button
    }

    // Show action buttons only if no action taken yet
    if (current->getLastAction() == ActionType::None) {

        // Basic actions
        addButton("Gather", [=]() {
            try {
                current->gather();
                showMessage(current->getName() + " gathered 1 coin.");
                updateButtons(); // Refresh to show End Turn
            } catch (const std::exception &e) {
                showPopup("Error: " + std::string(e.what()));
            }
        });

        addButton("Tax", [=]() {
            try {
                current->tax();
                showMessage(current->getName() + " collected " + std::to_string(current->taxAmount()) + " coins from tax.");
                updateButtons(); // Refresh to show End Turn
            } catch (const std::exception &e) {
                showPopup("Error: " + std::string(e.what()));
            }
        });

        // Targeted actions - single buttons that open target selection
        std::vector<Player *> validTargets = getValidTargets(current);

        if (!validTargets.empty()) {
            addButton("Arrest Player", [=]() {
                showTargetSelection("arrest", validTargets, [=](Player* target) {
                    try {
                        current->arrest(*target);
                        showMessage(current->getName() + " arrested " + target->getName() + ".");
                        updateButtons();
                    } catch (const std::exception &e) {
                        showPopup("Error: " + std::string(e.what()));
                    }
                });
            });

            addButton("Sanction Player", [=]() {
                showTargetSelection("sanction", validTargets, [=](Player* target) {
                    try {
                        current->sanction(*target);
                        showMessage(current->getName() + " sanctioned " + target->getName() + ".");
                        updateButtons();
                    } catch (const std::exception &e) {
                        showPopup("Error: " + std::string(e.what()));
                    }
                });
            });

            addButton("Coup Player", [=]() {
                showTargetSelection("coup", validTargets, [=](Player* target) {
                    try {
                        current->coup(*target);
                        showMessage(current->getName() + " couped " + target->getName() + "!");
                        turnInProgress = false;
                        waitingForBribeDecision = false;

                        std::cout << "[DEBUG] Coup completed, checking for winner..." << std::endl;
                        checkForWinner();
                        if (currentState != State::GameOver) {
                            updateButtons();
                        }
                    } catch (const std::exception &e) {
                        showPopup("Error: " + std::string(e.what()));
                    }
                });
            });
        }

        // Bribe action (if available)
        if (current->canUseBribe()) {
            addButton("Bribe", [=]() {
                try {
                    current->bribe();
                    if (current->getLastAction() == ActionType::Bribe) {
                        showMessage(current->getName() + " used bribe for bonus action!");
                        updateButtons();
                    } else {
                        showMessage(current->getName() + "'s bribe was blocked!");
                        turnInProgress = false;
                        waitingForBribeDecision = false;
                        checkForWinner();
                        if (currentState != State::GameOver) {
                            updateButtons();
                        }
                    }
                } catch (const std::exception &e) {
                    showPopup("Error: " + std::string(e.what()));
                }
            });
        }
    }

    // Show special abilities section at bottom
    drawSpecialAbilities(current, validTargets);
}

void CoupGUI::showTargetSelection(const std::string& actionName, const std::vector<Player*>& targets, std::function<void(Player*)> onTargetSelected) {
    // Store the callback for later use
    targetSelectionCallback = onTargetSelected;

    // Create a popup-style target selection
    std::string message = "Choose target to " + actionName + ":\n\n";
    for (size_t i = 0; i < targets.size(); ++i) {
        message += std::to_string(i + 1) + ". " + targets[i]->getName() + " (" + targets[i]->getRoleName() + ")\n";
    }
    message += "\nPress 1-" + std::to_string(targets.size()) + " to select target";

    // For now, just show popup (we'll implement proper selection later)
    if (!targets.empty()) {
        onTargetSelected(targets[0]); // Select first target for now
    }
}

void CoupGUI::drawSpecialAbilities(Player* current, const std::vector<Player*>& validTargets) {
    // Draw special abilities section at bottom
    sf::Text specialLabel;
    specialLabel.setFont(font);
    specialLabel.setString("Special Abilities (" + current->getRoleName() + "):");
    specialLabel.setCharacterSize(18);
    specialLabel.setFillColor(sf::Color::Yellow);
    specialLabel.setPosition(50, 650);
    window.draw(specialLabel);

    // Role-specific abilities
    float abilityY = 680;
    float abilityX = 50;

    if (auto baron = dynamic_cast<Baron*>(current)) {
        if (baron->getCoins() >= 3) {
            Button investBtn(abilityX, abilityY, 120, 30, font, "Invest (3→6)", [=]() {
                try {
                    baron->invest();
                    showMessage(current->getName() + " invested (3 → 6 coins).");
                    updateButtons();
                } catch (const std::exception &e) {
                    showPopup("Error: " + std::string(e.what()));
                }
            });
            investBtn.draw(window);

            // Check if clicked
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (investBtn.isClicked(sf::Mouse::getPosition(window))) {
                    investBtn.onClick();
                }
            }
        }
    }

    if (auto spy = dynamic_cast<Spy*>(current)) {
        if (!validTargets.empty()) {
            Button peekBtn(abilityX, abilityY, 120, 30, font, "Peek Coins", [=]() {
                showTargetSelection("peek at", validTargets, [=](Player* target) {
                    try {
                        int coins = spy->peekCoins(*target);
                        showMessage(target->getName() + " has " + std::to_string(coins) + " coins.");
                    } catch (const std::exception &e) {
                        showPopup("Error: " + std::string(e.what()));
                    }
                });
            });
            peekBtn.draw(window);

            Button blockBtn(abilityX + 130, abilityY, 120, 30, font, "Block Arrest", [=]() {
                showTargetSelection("block arrest of", validTargets, [=](Player* target) {
                    try {
                        spy->blockNextArrest(*target);
                        showMessage("Blocked " + target->getName() + "'s next arrest.");
                    } catch (const std::exception &e) {
                        showPopup("Error: " + std::string(e.what()));
                    }
                });
            });
            blockBtn.draw(window);

            // Check clicks
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (peekBtn.isClicked(mousePos)) {
                    peekBtn.onClick();
                } else if (blockBtn.isClicked(mousePos)) {
                    blockBtn.onClick();
                }
            }
        }
    }

    if (auto gov = dynamic_cast<Governor*>(current)) {
        for (auto *p : players) {
            if (p && p != current && game->isAlive(*p) &&
                game->hasPendingAction() && game->getLastActor() == p &&
                game->getLastActionType() == ActionType::Tax) {

                Button undoBtn(abilityX, abilityY, 120, 30, font, "Undo Tax", [=]() {
                    try {
                        gov->undo(*p);
                        showMessage(current->getName() + " undid " + p->getName() + "'s tax.");
                        updateButtons();
                    } catch (const std::exception &e) {
                        showPopup("Error: " + std::string(e.what()));
                    }
                });
                undoBtn.draw(window);

                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    if (undoBtn.isClicked(sf::Mouse::getPosition(window))) {
                        undoBtn.onClick();
                    }
                }
                break;
            }
        }
    }
}

void CoupGUI::drawPlayerStatus() {
    // Draw current player limitations on the right side
    sf::Text statusLabel;
    statusLabel.setFont(font);
    statusLabel.setString("Current Status:");
    statusLabel.setCharacterSize(18);
    statusLabel.setFillColor(sf::Color::White);
    statusLabel.setPosition(1000, 50);
    window.draw(statusLabel);

    Player* current = getCurrentPlayer();
    if (current) {
        std::string statusText = "";
        float yPos = 80;

        if (current->isSanctioned()) {
            sf::Text sanctionText;
            sanctionText.setFont(font);
            sanctionText.setString("• SANCTIONED");
            sanctionText.setCharacterSize(16);
            sanctionText.setFillColor(sf::Color::Red);
            sanctionText.setPosition(1000, yPos);
            window.draw(sanctionText);
            yPos += 25;
        }

        // Add other status indicators as needed
        if (current->getCoins() >= 10) {
            sf::Text mustCoupText;
            mustCoupText.setFont(font);
            mustCoupText.setString("• MUST COUP!");
            mustCoupText.setCharacterSize(16);
            mustCoupText.setFillColor(sf::Color::Yellow);
            mustCoupText.setPosition(1000, yPos);
            window.draw(mustCoupText);
            yPos += 25;
        }
    }
}