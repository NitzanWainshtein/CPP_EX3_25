// GUI/scenes/GameScene.cpp
#include "GameScene.hpp"
#include "../core/ResourceManager.hpp"
#include "../../Players/Roles/Baron.hpp"
#include "../../Players/Roles/Spy.hpp"
#include "../../Players/Roles/Governor.hpp"

namespace coup {
namespace gui {

GameScene::GameScene(GameController* controller)
    : gameController(controller) {

    setupUI();

    // Set callbacks
    gameController->setMessageCallback([this](const std::string& msg) {
        showMessage(msg);
    });

    gameController->setErrorCallback([this](const std::string& msg) {
        messagePopup->show(msg);
    });

    gameController->setStateChangeCallback([this]() {
        updateGameState();
    });
}

void GameScene::setupUI() {
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

    // Exit button
    exitButton = std::make_unique<Button>(
        1180, 20, 80, 40, font, "Exit",
        [this]() {
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

    // Game info texts
    bankText.setFont(font);
    bankText.setCharacterSize(24);
    bankText.setFillColor(sf::Color::White);
    bankText.setPosition(50, 30);

    turnText.setFont(font);
    turnText.setCharacterSize(28);
    turnText.setFillColor(sf::Color::Yellow);
    turnText.setPosition(50, 60);

    statusText.setFont(font);
    statusText.setCharacterSize(20);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(50, 95);

    // Message text
    messageText.setFont(font);
    messageText.setCharacterSize(20);
    messageText.setFillColor(sf::Color(255, 255, 200));
    messageText.setPosition(50, 850);

    // Popup
    messagePopup = std::make_unique<Popup>(font);

    // Turn control buttons
    startTurnButton = std::make_unique<Button>(
        50, 140, 150, 40, font, "Start Turn",
        [this]() {
            gameController->startTurn();
        }
    );

    endTurnButton = std::make_unique<Button>(
        210, 140, 150, 40, font, "End Turn",
        [this]() {
            gameController->endTurn();
        }
    );

    // Bribe decision buttons
    bribeYesButton = std::make_unique<Button>(
        400, 140, 120, 40, font, "Use Bribe",
        [this]() {
            gameController->handleBribeDecision(true);
        }
    );
    bribeYesButton->setColors(
        sf::Color(100, 100, 50),
        sf::Color(120, 120, 70),
        sf::Color(80, 80, 30),
        sf::Color(50, 50, 50)
    );

    bribeNoButton = std::make_unique<Button>(
        530, 140, 120, 40, font, "Skip Bribe",
        [this]() {
            gameController->handleBribeDecision(false);
        }
    );
}

void GameScene::onEnter() {
    createPlayerCards();
    createActionButtons();
    updateGameState();
}

void GameScene::createPlayerCards() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    playerCards.clear();

    auto playerNames = gameController->getPlayerNames();
    float startX = 50;
    float startY = 200;
    float cardWidth = 200;
    float cardHeight = 120;
    float spacing = 210;

    int row = 0;
    int col = 0;

    for (const auto& name : playerNames) {
        PlayerCard card;

        float x = startX + col * spacing;
        float y = startY + row * (cardHeight + 20);

        // Background
        card.background.setSize({cardWidth, cardHeight});
        card.background.setPosition(x, y);
        card.background.setFillColor(sf::Color(60, 60, 80));
        card.background.setOutlineThickness(3);
        card.background.setOutlineColor(sf::Color(100, 100, 120));

        // Name
        card.nameText.setFont(font);
        card.nameText.setString(name);
        card.nameText.setCharacterSize(20);
        card.nameText.setFillColor(sf::Color::White);
        card.nameText.setPosition(x + 10, y + 10);
        card.nameText.setStyle(sf::Text::Bold);

        // Role
        Player* player = gameController->getPlayer(name);
        if (player) {
            card.roleText.setFont(font);
            card.roleText.setString("(" + player->getRoleName() + ")");
            card.roleText.setCharacterSize(16);
            card.roleText.setFillColor(sf::Color(200, 200, 200));
            card.roleText.setPosition(x + 10, y + 35);
        }

        // Coins
        card.coinsText.setFont(font);
        card.coinsText.setCharacterSize(18);
        card.coinsText.setFillColor(sf::Color::Yellow);
        card.coinsText.setPosition(x + 10, y + 60);

        // Status
        card.statusText.setFont(font);
        card.statusText.setCharacterSize(14);
        card.statusText.setFillColor(sf::Color(255, 100, 100));
        card.statusText.setPosition(x + 10, y + 85);

        playerCards.push_back(card);

        col++;
        if (col >= 6) {
            col = 0;
            row++;
        }
    }
}

void GameScene::createActionButtons() {
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    commonButtons.clear();
    specialButtons.clear();

    // Common actions
    float commonY = 450;
    float buttonWidth = 140;
    float buttonHeight = 40;
    float spacing = 150;

    commonButtons["gather"] = std::make_unique<Button>(
        50, commonY, buttonWidth, buttonHeight, font, "Gather",
        [this]() { onGather(); }
    );

    commonButtons["tax"] = std::make_unique<Button>(
        50 + spacing, commonY, buttonWidth, buttonHeight, font, "Tax",
        [this]() { onTax(); }
    );

    commonButtons["arrest"] = std::make_unique<Button>(
        50 + spacing * 2, commonY, buttonWidth, buttonHeight, font, "Arrest",
        [this]() { showTargetSelection("arrest"); }
    );

    commonButtons["sanction"] = std::make_unique<Button>(
        50 + spacing * 3, commonY, buttonWidth, buttonHeight, font, "Sanction",
        [this]() { showTargetSelection("sanction"); }
    );

    commonButtons["coup"] = std::make_unique<Button>(
        50 + spacing * 4, commonY, buttonWidth, buttonHeight, font, "Coup",
        [this]() { showTargetSelection("coup"); }
    );

    commonButtons["bribe"] = std::make_unique<Button>(
        50 + spacing * 5, commonY, buttonWidth, buttonHeight, font, "Bribe",
        [this]() { onBribe(); }
    );

    // Special abilities (will be shown based on current player's role)
    float specialY = 550;

    specialButtons["invest"] = std::make_unique<Button>(
        50, specialY, buttonWidth, buttonHeight, font, "Invest",
        [this]() { onInvest(); }
    );
    specialButtons["invest"]->setColors(
        sf::Color(100, 80, 50),
        sf::Color(120, 100, 70),
        sf::Color(80, 60, 30),
        sf::Color(50, 50, 50)
    );

    specialButtons["peek"] = std::make_unique<Button>(
        50, specialY, buttonWidth, buttonHeight, font, "Peek Coins",
        [this]() { showTargetSelection("peek"); }
    );

    specialButtons["blockArrest"] = std::make_unique<Button>(
        50 + spacing, specialY, buttonWidth, buttonHeight, font, "Block Arrest",
        [this]() { showTargetSelection("blockArrest"); }
    );

    specialButtons["undo"] = std::make_unique<Button>(
        50, specialY, buttonWidth, buttonHeight, font, "Undo Tax",
        [this]() { onUndo(); }
    );
}

void GameScene::updateGameState() {
    if (!gameController->isGameActive()) return;

    // Update bank
    bankText.setString("Bank: " + std::to_string(gameController->getBankCoins()) + " coins");

    // Update turn
    if (gameController->isGameOver()) {
        turnText.setString("Game Over! Winner: " + gameController->getWinner());
        transitionTo("GameOver");
        return;
    } else {
        turnText.setString("Current Turn: " + gameController->getCurrentPlayerName());
    }

    // Update player cards
    auto playerNames = gameController->getPlayerNames();
    for (size_t i = 0; i < playerCards.size() && i < playerNames.size(); ++i) {
        const std::string& name = playerNames[i];
        Player* player = gameController->getPlayer(name);

        if (player) {
            // Update coins
            playerCards[i].coinsText.setString(std::to_string(player->getCoins()) + " coins");

            // Update status
            std::string status = "";
            if (player->isSanctioned()) status = "SANCTIONED";
            playerCards[i].statusText.setString(status);

            // Update active state
            playerCards[i].isActive = (name == gameController->getCurrentPlayerName());
            playerCards[i].isAlive = gameController->isPlayerAlive(name);

            // Update colors
            if (!playerCards[i].isAlive) {
                playerCards[i].background.setFillColor(sf::Color(40, 40, 40));
                playerCards[i].nameText.setFillColor(sf::Color(100, 100, 100));
            } else if (playerCards[i].isActive) {
                playerCards[i].background.setOutlineColor(sf::Color::Yellow);
                playerCards[i].background.setOutlineThickness(4);
            } else {
                playerCards[i].background.setOutlineColor(sf::Color(100, 100, 120));
                playerCards[i].background.setOutlineThickness(3);
            }
        }
    }

    // Update buttons
    updateButtons();

    // Check for forced coup
    Player* current = gameController->getCurrentPlayer();
    if (current && current->getCoins() >= 10 && gameController->canStartTurn()) {
        handleForcedCoup();
    }
}

void GameScene::updateButtons() {
    bool canStart = gameController->canStartTurn();
    bool inProgress = gameController->isTurnInProgress();
    bool waitingBribe = gameController->isWaitingForBribe();

    // Turn control
    startTurnButton->setEnabled(canStart && !inProgress);
    endTurnButton->setEnabled(inProgress && !waitingBribe);

    // Bribe decision
    bribeYesButton->setEnabled(waitingBribe);
    bribeNoButton->setEnabled(waitingBribe);

    // Common actions
    for (auto& [name, btn] : commonButtons) {
        btn->setEnabled(inProgress && !waitingBribe && !selectingTarget);
    }

    // Special abilities based on role
    Player* current = gameController->getCurrentPlayer();
    if (current) {
        std::string role = current->getRoleName();

        // Hide all special buttons first
        for (auto& [name, btn] : specialButtons) {
            btn->setEnabled(false);
        }

        // Show relevant ones
        if (role == "Baron" && current->getCoins() >= 3) {
            specialButtons["invest"]->setEnabled(inProgress && !waitingBribe);
        } else if (role == "Spy") {
            specialButtons["peek"]->setEnabled(true); // Always available
            specialButtons["blockArrest"]->setEnabled(true);
        } else if (role == "Governor") {
            // Check if there's a tax to undo
            specialButtons["undo"]->setEnabled(true);
        }
    }
}

void GameScene::showTargetSelection(const std::string& action) {
    selectingTarget = true;
    pendingAction = action;
    targetButtons.clear();

    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    auto playerNames = gameController->getPlayerNames();
    std::string currentName = gameController->getCurrentPlayerName();

    float startY = 650;
    float buttonWidth = 150;
    float spacing = 160;
    int col = 0;

    for (const auto& name : playerNames) {
        if (name != currentName && gameController->isPlayerAlive(name)) {
            auto btn = std::make_unique<Button>(
                50 + col * spacing, startY, buttonWidth, 40, font, name,
                [this, name, action]() {
                    hideTargetSelection();

                    if (action == "arrest") {
                        onArrest(name);
                    } else if (action == "sanction") {
                        onSanction(name);
                    } else if (action == "coup") {
                        onCoup(name);
                    } else if (action == "peek") {
                        onPeek(name);
                    } else if (action == "blockArrest") {
                        onBlockArrest(name);
                    }
                }
            );
            targetButtons.push_back(std::move(btn));
            col++;
        }
    }

    // Cancel button
    auto cancelBtn = std::make_unique<Button>(
        50 + col * spacing, startY, 100, 40, font, "Cancel",
        [this]() { hideTargetSelection(); }
    );
    cancelBtn->setColors(
        sf::Color(120, 60, 60),
        sf::Color(150, 80, 80),
        sf::Color(90, 40, 40),
        sf::Color(50, 50, 50)
    );
    targetButtons.push_back(std::move(cancelBtn));
}

void GameScene::hideTargetSelection() {
    selectingTarget = false;
    pendingAction.clear();
    targetButtons.clear();
}

void GameScene::handleForcedCoup() {
    showMessage("You have 10+ coins - YOU MUST COUP!", 5.0f);
    statusText.setString("FORCED COUP - Select target!");
    statusText.setFillColor(sf::Color::Red);

    // Disable all buttons except coup targets
    for (auto& [name, btn] : commonButtons) {
        btn->setEnabled(false);
    }
    for (auto& [name, btn] : specialButtons) {
        btn->setEnabled(false);
    }

    // Auto-show coup targets
    showTargetSelection("coup");
}

void GameScene::showMessage(const std::string& msg, float duration) {
    messageText.setString(msg);
    messageTimer = duration;
}

// Action implementations
void GameScene::onGather() {
    gameController->performGather();
}

void GameScene::onTax() {
    gameController->performTax();
}

void GameScene::onBribe() {
    gameController->performBribe();
}

void GameScene::onArrest(const std::string& target) {
    gameController->performArrest(target);
}

void GameScene::onSanction(const std::string& target) {
    gameController->performSanction(target);
}

void GameScene::onCoup(const std::string& target) {
    gameController->performCoup(target);
}

void GameScene::onInvest() {
    gameController->performInvest();
}

void GameScene::onPeek(const std::string& target) {
    gameController->peekCoins(target);
}

void GameScene::onBlockArrest(const std::string& target) {
    gameController->blockNextArrest(target);
}

void GameScene::onUndo() {
    // Find who did the last tax
    if (gameController->getCurrentPlayer() &&
        gameController->getCurrentPlayer()->getRoleName() == "Governor") {

        // For simplicity, we'll need to add a method to GameController
        // to track last tax player, or show a target selection
        showMessage("Select player whose tax to undo");
        showTargetSelection("undo");
    }
}

void GameScene::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    // Handle popup first
    if (messagePopup->isVisible()) {
        messagePopup->handleEvent(event, window);
        return;
    }

    // Exit button
    exitButton->handleEvent(event, window);

    // Turn controls
    startTurnButton->handleEvent(event, window);
    endTurnButton->handleEvent(event, window);
    bribeYesButton->handleEvent(event, window);
    bribeNoButton->handleEvent(event, window);

    // Target selection
    if (selectingTarget) {
        for (auto& btn : targetButtons) {
            btn->handleEvent(event, window);
        }
        return;
    }

    // Action buttons
    for (auto& [name, btn] : commonButtons) {
        btn->handleEvent(event, window);
    }

    // Special ability buttons (only show for current player)
    Player* current = gameController->getCurrentPlayer();
    if (current) {
        std::string role = current->getRoleName();

        if (role == "Baron" && specialButtons["invest"]->isEnabled()) {
            specialButtons["invest"]->handleEvent(event, window);
        } else if (role == "Spy") {
            if (specialButtons["peek"]->isEnabled()) {
                specialButtons["peek"]->handleEvent(event, window);
            }
            if (specialButtons["blockArrest"]->isEnabled()) {
                specialButtons["blockArrest"]->handleEvent(event, window);
            }
        } else if (role == "Governor" && specialButtons["undo"]->isEnabled()) {
            specialButtons["undo"]->handleEvent(event, window);
        }
    }
}

void GameScene::update(float deltaTime, const sf::RenderWindow& window) {
    // Update message timer
    if (messageTimer > 0) {
        messageTimer -= deltaTime;
        if (messageTimer <= 0) {
            messageText.setString("");
        }
    }

    // Update buttons
    exitButton->update(window);
    startTurnButton->update(window);
    endTurnButton->update(window);
    bribeYesButton->update(window);
    bribeNoButton->update(window);
    messagePopup->update(window);

    // Update action buttons
    for (auto& [name, btn] : commonButtons) {
        btn->update(window);
    }

    // Update special buttons for current player
    Player* current = gameController->getCurrentPlayer();
    if (current) {
        std::string role = current->getRoleName();

        if (role == "Baron") {
            specialButtons["invest"]->update(window);
        } else if (role == "Spy") {
            specialButtons["peek"]->update(window);
            specialButtons["blockArrest"]->update(window);
        } else if (role == "Governor") {
            specialButtons["undo"]->update(window);
        }
    }

    // Update target buttons
    for (auto& btn : targetButtons) {
        btn->update(window);
    }

    // Update status text
    if (!selectingTarget && !gameController->isWaitingForBribe()) {
        if (gameController->canStartTurn()) {
            statusText.setString("Click 'Start Turn' to begin");
            statusText.setFillColor(sf::Color::White);
        } else if (gameController->isTurnInProgress()) {
            statusText.setString("Choose your action");
            statusText.setFillColor(sf::Color::White);
        }
    } else if (gameController->isWaitingForBribe()) {
        statusText.setString("Decide: Use bribe for bonus action?");
        statusText.setFillColor(sf::Color::Yellow);
    }
}

void GameScene::render(sf::RenderWindow& window) {
    window.clear(sf::Color(30, 30, 40));

    // Background
    window.draw(background);

    // Game info
    window.draw(bankText);
    window.draw(turnText);
    window.draw(statusText);

    // Player cards
    for (const auto& card : playerCards) {
        if (card.isAlive || true) { // Show all players, even eliminated
            window.draw(card.background);
            window.draw(card.nameText);
            window.draw(card.roleText);
            window.draw(card.coinsText);
            window.draw(card.statusText);
        }
    }

    // Section labels
    auto& rm = ResourceManager::getInstance();
    sf::Font& font = rm.getFont("main");

    sf::Text commonLabel;
    commonLabel.setFont(font);
    commonLabel.setString("Common Actions:");
    commonLabel.setCharacterSize(18);
    commonLabel.setFillColor(sf::Color::White);
    commonLabel.setPosition(50, 420);
    window.draw(commonLabel);

    // Draw action buttons
    for (auto& [name, btn] : commonButtons) {
        if (btn->isEnabled() || gameController->isTurnInProgress()) {
            btn->draw(window);
        }
    }

    // Draw special ability buttons
    Player* current = gameController->getCurrentPlayer();
    if (current && gameController->isTurnInProgress()) {
        std::string role = current->getRoleName();

        sf::Text specialLabel;
        specialLabel.setFont(font);
        specialLabel.setString("Special Abilities (" + role + "):");
        specialLabel.setCharacterSize(18);
        specialLabel.setFillColor(sf::Color::Yellow);
        specialLabel.setPosition(50, 520);
        window.draw(specialLabel);

        if (role == "Baron") {
            specialButtons["invest"]->draw(window);
        } else if (role == "Spy") {
            specialButtons["peek"]->draw(window);
            specialButtons["blockArrest"]->draw(window);
        } else if (role == "Governor") {
            specialButtons["undo"]->draw(window);
        }
    }

    // Target selection
    if (selectingTarget) {
        sf::Text targetLabel;
        targetLabel.setFont(font);
        targetLabel.setString("Select Target:");
        targetLabel.setCharacterSize(20);
        targetLabel.setFillColor(sf::Color::White);
        targetLabel.setPosition(50, 620);
        window.draw(targetLabel);

        for (auto& btn : targetButtons) {
            btn->draw(window);
        }
    }

    // Turn controls
    startTurnButton->draw(window);
    endTurnButton->draw(window);

    if (gameController->isWaitingForBribe()) {
        bribeYesButton->draw(window);
        bribeNoButton->draw(window);
    }

    // Exit button
    exitButton->draw(window);

    // Message
    if (messageTimer > 0) {
        window.draw(messageText);
    }

    // Popup (last, on top)
    messagePopup->draw(window);
}

} // namespace gui
} // namespace coup