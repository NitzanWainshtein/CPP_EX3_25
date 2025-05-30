// Email: nitzanwa@gmail.com

#include "GUI.hpp"
#include "../Players/Roles/Baron.hpp"
#include "../Players/Roles/General.hpp"
#include "../Players/Roles/Governor.hpp"
#include "../Players/Roles/Judge.hpp"
#include "../Players/Roles/Merchant.hpp"
#include "../Players/Roles/Spy.hpp"
#include "../GameLogic/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <codecvt>
#include <locale>

// הפונקציה שמחזירה שחקן רנדומלי
namespace coup {
    extern Player* randomPlayer(Game& game, const std::string& name);
}

// Button implementation
Button::Button(float x, float y, float width, float height,
               const sf::Font& font, const std::string& label,
               std::function<void()> onClick)
    : onClick(std::move(onClick)), enabled(true) {
    shape.setPosition(x, y);
    shape.setSize({width, height});
    shape.setFillColor(sf::Color(90, 90, 90));
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(x + (width - textBounds.width) / 2 - textBounds.left,
                     y + (height - textBounds.height) / 2 - textBounds.top);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::isClicked(sf::Vector2i mousePos) {
    return enabled && shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Button::setEnabled(bool enabled) {
    this->enabled = enabled;
    if (enabled) {
        shape.setFillColor(sf::Color(90, 90, 90));
        text.setFillColor(sf::Color::White);
    } else {
        shape.setFillColor(sf::Color(60, 60, 60));
        text.setFillColor(sf::Color(100, 100, 100));
    }
}

void Button::setSelected(bool selected) {
    if (selected) {
        shape.setFillColor(sf::Color(120, 120, 170));
        shape.setOutlineColor(sf::Color::Cyan);
    } else {
        shape.setFillColor(sf::Color(90, 90, 90));
        shape.setOutlineColor(sf::Color::White);
    }
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(x + (shape.getSize().x - textBounds.width) / 2 - textBounds.left,
                     y + (shape.getSize().y - textBounds.height) / 2 - textBounds.top);
}

sf::Vector2f Button::getSize() const {
    return shape.getSize();
}

void Button::setLabel(const std::string& newLabel) {
    text.setString(newLabel);
    sf::FloatRect textBounds = text.getLocalBounds();
    sf::Vector2f pos = shape.getPosition();
    sf::Vector2f size = shape.getSize();
    text.setPosition(pos.x + (size.x - textBounds.width) / 2 - textBounds.left,
                     pos.y + (size.y - textBounds.height) / 2 - textBounds.top);
}

// PlayerCard implementation
PlayerCard::PlayerCard(coup::Player* player, const sf::Font& font)
: player(player) {
    background.setSize({250.f, 120.f});
    background.setFillColor(sf::Color(60, 60, 60, 200));
    background.setOutlineThickness(2);
    background.setOutlineColor(sf::Color::White);

    nameText.setFont(font);
    nameText.setCharacterSize(18);
    nameText.setFillColor(sf::Color::White);

    roleText.setFont(font);
    roleText.setCharacterSize(16);
    roleText.setFillColor(sf::Color::Yellow);

    coinsText.setFont(font);
    coinsText.setCharacterSize(16);
    coinsText.setFillColor(sf::Color::Green);

    statusText.setFont(font);
    statusText.setCharacterSize(14);
    statusText.setFillColor(sf::Color::Red);

    update();
}

void PlayerCard::update() {
    if (!player) return;

    nameText.setString(player->getName());
    roleText.setString("Role: " + player->getRoleName());
    coinsText.setString("Coins: " + std::to_string(player->getCoins()));

    std::string status;
    if (player->isSanctioned()) {
        status += "[SANCTIONED]";
    }

    // Add arrest status
    coup::ArrestStatus arrestStatus = player->getArrestStatus();
    if (arrestStatus == coup::ArrestStatus::ArrestedNow) {
        if (!status.empty()) status += " ";
        status += "[ARRESTED]";
    } else if (arrestStatus == coup::ArrestStatus::Cooldown) {
        if (!status.empty()) status += " ";
        status += "[COOLDOWN]";
    }

    // Add arrest blocked status
    if (player->isArrestBlocked()) {
        if (!status.empty()) status += " ";
        status += "[ARREST BLOCKED]";
    }

    statusText.setString(status);
}

void PlayerCard::draw(sf::RenderWindow& window, bool isCurrentPlayer) {
    if (isCurrentPlayer) {
        background.setFillColor(sf::Color(90, 90, 120, 220));
        background.setOutlineColor(sf::Color::Yellow);
    } else {
        background.setFillColor(sf::Color(60, 60, 60, 200));
        background.setOutlineColor(sf::Color::White);
    }

    window.draw(background);
    window.draw(nameText);
    window.draw(roleText);
    window.draw(coinsText);
    if (!statusText.getString().isEmpty()) {
        window.draw(statusText);
    }
}

void PlayerCard::setPosition(float x, float y) {
    background.setPosition(x, y);
    nameText.setPosition(x + 10, y + 10);
    roleText.setPosition(x + 10, y + 35);
    coinsText.setPosition(x + 10, y + 60);
    statusText.setPosition(x + 10, y + 85);
}

sf::FloatRect PlayerCard::getBounds() const {
    return background.getGlobalBounds();
}

// Message popup implementation
PopupMessage::PopupMessage(const sf::Font& font) : isActive(false), timer(0) {
    background.setSize({600.f, 150.f});
    background.setFillColor(sf::Color(50, 50, 50, 230));
    background.setOutlineThickness(3);

    messageText.setFont(font);
    messageText.setCharacterSize(22);

    okButton = std::make_unique<Button>(0, 0, 80, 40, font, "OK", [this]() {
        this->isActive = false;
    });
}

void PopupMessage::show(const std::string& message, bool isError) {
    isActive = true;
    timer = 0;
    messageText.setString(message);

    if (isError) {
        background.setOutlineColor(sf::Color::Red);
        messageText.setFillColor(sf::Color::Red);
    } else {
        background.setOutlineColor(sf::Color::White);
        messageText.setFillColor(sf::Color::White);
    }

    // Center the text
    sf::FloatRect textBounds = messageText.getLocalBounds();
    messageText.setPosition(
        (600.f - textBounds.width) / 2 - textBounds.left,
        (150.f - textBounds.height) / 3 - textBounds.top
    );

    // Position the button
    okButton->setPosition(260.f, 90.f);
}

void PopupMessage::update(float dt) {
    timer += dt;
    if (timer > 10.0f) { // Auto-dismiss after 10 seconds
        isActive = false;
    }
}

void PopupMessage::draw(sf::RenderWindow& window) {
    if (!isActive) return;

    // Center the popup on the screen
    sf::Vector2f center = sf::Vector2f(window.getSize()) / 2.0f;
    background.setPosition(center.x - 300.f, center.y - 75.f);

    sf::Vector2f bgPos = background.getPosition();
    messageText.setPosition(
        bgPos.x + (600.f - messageText.getLocalBounds().width) / 2 - messageText.getLocalBounds().left,
        bgPos.y + (150.f - messageText.getLocalBounds().height) / 3 - messageText.getLocalBounds().top
    );

    okButton->setPosition(bgPos.x + 260.f, bgPos.y + 90.f);

    window.draw(background);
    window.draw(messageText);
    okButton->draw(window);
}

bool PopupMessage::handleClick(sf::Vector2i mousePos) {
    if (!isActive) return false;

    if (okButton->isClicked(mousePos)) {
        okButton->onClick();
        return true;
    }
    return false;
}

// Block action popup implementation
BlockActionPopup::BlockActionPopup(const sf::Font& font) : isActive(false), 
    blocker(nullptr), actor(nullptr), 
    action(coup::ActionType::None), target(nullptr) {
    background.setSize({700.f, 200.f});
    background.setFillColor(sf::Color(50, 50, 50, 230));
    background.setOutlineThickness(3);
    background.setOutlineColor(sf::Color::Yellow);

    titleText.setFont(font);
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setString("BLOCKING OPPORTUNITY");

    messageText.setFont(font);
    messageText.setCharacterSize(20);
    messageText.setFillColor(sf::Color::White);

    yesButton = std::make_unique<Button>(0, 0, 120, 40, font, "Yes, Block", nullptr);
    noButton = std::make_unique<Button>(0, 0, 120, 40, font, "No, Allow", nullptr);
}

void BlockActionPopup::show(coup::Player* blocker, coup::Player* actor, 
    coup::ActionType action, coup::Player* target,
    std::function<void(bool)> callback) {
    isActive = true;
    this->blocker = blocker;
    this->actor = actor;
    this->action = action;
    this->target = target;
    this->callback = callback;

    std::string actionName;
    switch (action) {
    case coup::ActionType::Gather: actionName = "Gather"; break;
    case coup::ActionType::Tax: actionName = "Tax"; break;
    case coup::ActionType::Bribe: actionName = "Bribe"; break;
    case coup::ActionType::Arrest: actionName = "Arrest"; break;
    case coup::ActionType::Sanction: actionName = "Sanction"; break;
    case coup::ActionType::Coup: actionName = "Coup"; break;
    case coup::ActionType::Invest: actionName = "Invest"; break;
    default: actionName = "Unknown"; break;
    }

    std::string targetName = target ? target->getName() : "";
    std::string msg = blocker->getName() + " (" + blocker->getRoleName() + 
        "), do you want to block " + actor->getName() +
        "'s " + actionName;

    if (target) {
        msg += " on " + targetName;
    }

    msg += "?";

    messageText.setString(msg);

    yesButton->onClick = [this]() {
        if (this->callback) {
            this->callback(true);
        }
        this->isActive = false;
    };

    noButton->onClick = [this]() {
        if (this->callback) {
            this->callback(false);
        }
        this->isActive = false;
    };
}

void BlockActionPopup::draw(sf::RenderWindow& window) {
    if (!isActive) return;

    // Center the popup on the screen
    sf::Vector2f center = sf::Vector2f(window.getSize()) / 2.0f;
    background.setPosition(center.x - 350.f, center.y - 100.f);

    sf::Vector2f bgPos = background.getPosition();

    titleText.setPosition(
        bgPos.x + (700.f - titleText.getLocalBounds().width) / 2 - titleText.getLocalBounds().left,
        bgPos.y + 20.f
    );

    messageText.setPosition(
        bgPos.x + (700.f - messageText.getLocalBounds().width) / 2 - messageText.getLocalBounds().left,
        bgPos.y + 70.f
    );

    yesButton->setPosition(bgPos.x + 200.f, bgPos.y + 130.f);
    noButton->setPosition(bgPos.x + 380.f, bgPos.y + 130.f);

    window.draw(background);
    window.draw(titleText);
    window.draw(messageText);
    yesButton->draw(window);
    noButton->draw(window);
}

bool BlockActionPopup::handleClick(sf::Vector2i mousePos) {
    if (!isActive) return false;

    if (yesButton->isClicked(mousePos)) {
        yesButton->onClick();
        return true;
    }

    if (noButton->isClicked(mousePos)) {
        noButton->onClick();
        return true;
    }

    return false;
}

// Spy abilities panel implementation
SpyAbilitiesPanel::SpyAbilitiesPanel(const sf::Font& font) : isVisible(false), viewCoinsMode(false) {
    background.setSize({200.f, 190.f});
    background.setFillColor(sf::Color(40, 40, 60, 230));
    background.setOutlineThickness(2);
    background.setOutlineColor(sf::Color::Cyan);

    titleText.setFont(font);
    titleText.setCharacterSize(18);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setString("Spy Abilities");

    viewCoinsButton = std::make_unique<Button>(0, 0, 180, 40, font, "View Coins", nullptr);
    blockArrestButton = std::make_unique<Button>(0, 0, 180, 40, font, "Block Next Arrest", nullptr);
    closeButton = std::make_unique<Button>(0, 0, 180, 40, font, "Close", [this]() {
        this->isVisible = false;
    });
}

void SpyAbilitiesPanel::show(std::function<void(bool)> spyActionCallback) {
    isVisible = true;

    viewCoinsButton->onClick = [this, spyActionCallback]() {
        viewCoinsMode = true;
        isVisible = false;  // Auto-hide panel after selection
        spyActionCallback(true);
    };

    blockArrestButton->onClick = [this, spyActionCallback]() {
        viewCoinsMode = false;
        isVisible = false;  // Auto-hide panel after selection
        spyActionCallback(false);
    };
}

void SpyAbilitiesPanel::setPosition(float x, float y) {
    background.setPosition(x, y);

    titleText.setPosition(
        x + (200.f - titleText.getLocalBounds().width) / 2 - titleText.getLocalBounds().left,
        y + 10.f
    );

    viewCoinsButton->setPosition(x + 10.f, y + 40.f);
    blockArrestButton->setPosition(x + 10.f, y + 90.f);
    closeButton->setPosition(x + 10.f, y + 140.f);
}

void SpyAbilitiesPanel::draw(sf::RenderWindow& window) {
    if (!isVisible) return;

    window.draw(background);
    window.draw(titleText);
    viewCoinsButton->draw(window);
    blockArrestButton->draw(window);
    closeButton->draw(window);
}

bool SpyAbilitiesPanel::handleClick(sf::Vector2i mousePos) {
    if (!isVisible) return false;

    if (viewCoinsButton->isClicked(mousePos)) {
        viewCoinsButton->onClick();
        return true;
    }

    if (blockArrestButton->isClicked(mousePos)) {
        blockArrestButton->onClick();
        return true;
    }

    if (closeButton->isClicked(mousePos)) {
        closeButton->onClick();
        return true;
    }

    return false;
}

// GUI implementation
GUI::GUI()
: currentState(State::MainMenu),
  actionState(ActionState::None),
  pendingAction(coup::ActionType::None),
  startedCurrentTurn(false),
  lastPlayerName(""),
  window(sf::VideoMode(1280, 720), "Coup Game"),
  game(nullptr),
  playerCount(0),
  currentNameIndex(0),
  inputFocus(true),
  hasPerformedAction(false),
  messageTimer(0.0f),
  cachedWinnerName("") {  // הוספת האיתחול החדש
    window.setFramerateLimit(60);
    playerNames.clear();
    loadAssets();
    setupMainMenu();

    popup = std::make_unique<PopupMessage>(font);
    blockPopup = std::make_unique<BlockActionPopup>(font);
    spyPanel = std::make_unique<SpyAbilitiesPanel>(font);

    // Position the spy panel
    spyPanel->setPosition(1050.f, 200.f);
}

GUI::~GUI() {
    // ניקוי משאבי המשחק בצורה בטוחה
    if (game) {
        std::cout << "Cleaning up game resources in destructor" << std::endl;
        try {
            delete game;
        } catch (const std::exception& e) {
            std::cerr << "Error during game cleanup: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error during game cleanup" << std::endl;
        }
        game = nullptr;
        playerCards.clear();
        actionButtons.clear();
        arrestBlockedPlayers.clear();
    }
}

void GUI::showMessage(const std::string& msg) {
    messageText.setFillColor(sf::Color::White);
    messageText.setString(msg);
    messageTimer = MESSAGE_DURATION;
}

void GUI::showErrorPopup(const std::string& msg) {
    popup->show(msg, true);
}

void GUI::showInfoPopup(const std::string& msg) {
    popup->show(msg, false);
}

bool GUI::hasUsedBribe(coup::Player* player) const {
    return player && player->getLastAction() == coup::ActionType::Bribe;
}

void GUI::updateButtons() {
    // Buttons are recreated in each draw function
}

bool GUI::checkForBlockers(coup::Player* actor, coup::ActionType action, coup::Player* /* target */) {
    if (!game || !actor) return false;

    // Handle blocking for different actions
    if (action == coup::ActionType::Bribe) {
        // Check for Judge who can block bribe
        for (auto* p : game->getAllAlivePlayers()) {
            if (p && p->getRoleName() == "Judge" && p != actor) {
                bool shouldBlock = false;
                bool decided = false;
                
                actionState = ActionState::WaitingForBlock;
                
                blockPopup->show(p, actor, action, nullptr, [this, &shouldBlock, &decided](bool block) {
                    shouldBlock = block;
                    decided = true;
                });
                
                // Wait for response
                while (!decided && window.isOpen()) {
                    sf::Event event;
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed) {
                            window.close();
                            return false;
                        }
                        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            blockPopup->handleClick(mousePos);
                        }
                    }
                    
                    // Update and render
                    window.clear(sf::Color::Black);
                    window.draw(backgroundSprite);
                    
                    for (auto& card : playerCards) {
                        bool isCurrentPlayer = (card.getPlayer() == actor);
                        card.draw(window, isCurrentPlayer);
                    }
                    
                    blockPopup->draw(window);
                    window.display();
                }
                
                actionState = ActionState::None;
                
                if (shouldBlock) {
                    showMessage(p->getName() + " (Judge) blocked " + actor->getName() + "'s bribe!");
                    return true;
                }
                
                break; // Only one Judge can block
            }
        }
    }
    else if (action == coup::ActionType::Tax) {
        // Check for Governor who can block tax
        for (auto* p : game->getAllAlivePlayers()) {
            if (p && p->getRoleName() == "Governor" && p != actor) {
                bool shouldBlock = false;
                bool decided = false;
                
                actionState = ActionState::WaitingForBlock;
                
                blockPopup->show(p, actor, action, nullptr, [this, &shouldBlock, &decided](bool block) {
                    shouldBlock = block;
                    decided = true;
                });
                
                // Wait for response
                while (!decided && window.isOpen()) {
                    sf::Event event;
                    while (window.pollEvent(event)) {
                        if (event.type == sf::Event::Closed) {
                            window.close();
                            return false;
                        }
                        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                            blockPopup->handleClick(mousePos);
                        }
                    }
                    
                    // Update and render
                    window.clear(sf::Color::Black);
                    window.draw(backgroundSprite);
                    
                    for (auto& card : playerCards) {
                        bool isCurrentPlayer = (card.getPlayer() == actor);
                        card.draw(window, isCurrentPlayer);
                    }
                    
                    blockPopup->draw(window);
                    window.display();
                }
                
                actionState = ActionState::None;
                
                if (shouldBlock) {
                    showMessage(p->getName() + " (Governor) blocked " + actor->getName() + "'s tax collection!");
                    return true;
                }
                
                break; // Only one Governor can block
            }
        }
    }

    return false;
}

coup::Player* GUI::findPotentialBlocker(coup::ActionType action) {
    if (!game) return nullptr;
    
    if (action == coup::ActionType::Bribe) {
        for (auto* p : game->getAllAlivePlayers()) {
            if (p && p->getRoleName() == "Judge") {
                return p;
            }
        }
    }
    else if (action == coup::ActionType::Tax) {
        for (auto* p : game->getAllAlivePlayers()) {
            if (p && p->getRoleName() == "Governor") {
                return p;
            }
        }
    }
    
    return nullptr;
}

bool GUI::showYesNoPopup(const std::string& message) {
    bool result = false;
    bool decided = false;

    blockPopup->isActive = true;
    blockPopup->messageText.setString(message);
    
    blockPopup->yesButton->onClick = [&decided, &result]() {
        result = true;
        decided = true;
    };
    
    blockPopup->noButton->onClick = [&decided, &result]() {
        result = false;
        decided = true;
    };
    
    while (!decided && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                blockPopup->handleClick(mousePos);
            }
        }
        
        window.clear(sf::Color::Black);
        window.draw(backgroundSprite);
        blockPopup->draw(window);
        window.display();
    }
    
    blockPopup->isActive = false;
    return result;
}

std::string GUI::sanitizeInput(const std::string& input) {
    std::string result;
    for (unsigned char c : input) {
        // Only allow ASCII printable characters
        if (c >= 32 && c < 127) {
            result += c;
        }
    }
    return result;
}

bool GUI::isValidUtf8(const std::string& str) {
    for (size_t i = 0; i < str.length(); ) {
        unsigned char c = str[i];
        
        // ASCII
        if (c <= 0x7F) {
            i++;
        }
        // Multi-byte UTF-8 - we'll skip these
        else {
            return false;
        }
    }
    return true;
}

bool GUI::isPlayerArrestBlocked(const std::string& playerName) const {
    auto it = arrestBlockedPlayers.find(playerName);
    return it != arrestBlockedPlayers.end() && it->second > 0;
}

void GUI::updateArrestBlocks() {
    // Decrement arrest block counters
    for (auto& pair : arrestBlockedPlayers) {
        if (pair.second > 0) {
            pair.second--;
        }
    }
    
    // Remove expired blocks
    auto it = arrestBlockedPlayers.begin();
    while (it != arrestBlockedPlayers.end()) {
        if (it->second <= 0) {
            it = arrestBlockedPlayers.erase(it);
        } else {
            ++it;
        }
    }
}

void GUI::loadAssets() {
    // כאן התיקון: הסרת המשתנה שלא משמש fontLoaded
    if (font.loadFromFile("GUI/fonts/ARIAL.TTF")) {
        // יש הטענה מוצלחת
    } else if (font.loadFromFile("./GUI/fonts/ARIAL.TTF")) {
        // יש הטענה מוצלחת
    } else if (font.loadFromFile("Arial.ttf")) {
        // יש הטענה מוצלחת
    } else {
        // If we couldn't load the font, try to load any system font
        std::cerr << "Failed to load font, using system font\n";
        
        // Try common fonts on different systems
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            // יש הטענה מוצלחת
        } else if (font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            // יש הטענה מוצלחת
        } else {
            std::cerr << "Could not load any font, using default\n";
            // If we couldn't load a font, use SFML's default
        }
    }

    if (!menuBackgroundTexture.loadFromFile("GUI/images/background_menu.png")) {
        std::cerr << "Failed to load menu background! Using fallback.\n";
        menuBackgroundTexture.create(1280, 720);
        sf::Uint8* pixels = new sf::Uint8[1280 * 720 * 4];
        for (int i = 0; i < 1280 * 720; ++i) {
            pixels[i*4] = 30;
            pixels[i*4+1] = 30;
            pixels[i*4+2] = 50;
            pixels[i*4+3] = 255;
        }
        menuBackgroundTexture.update(pixels);
        delete[] pixels;
    }
    
    if (!gameBackgroundTexture.loadFromFile("GUI/images/game_background.png")) {
        std::cerr << "Failed to load game background! Using fallback.\n";
        gameBackgroundTexture.create(1280, 720);
        sf::Uint8* pixels = new sf::Uint8[1280 * 720 * 4];
        for (int i = 0; i < 1280 * 720; ++i) {
            pixels[i*4] = 20;
            pixels[i*4+1] = 40;
            pixels[i*4+2] = 40;
            pixels[i*4+3] = 255;
        }
        gameBackgroundTexture.update(pixels);
        delete[] pixels;
    }
    
    // Properly load and use winner background
    if (!winnerBackgroundTexture.loadFromFile("GUI/images/winner_bg.png")) {
        std::cerr << "Failed to load winner background! Using fallback.\n";
        winnerBackgroundTexture = gameBackgroundTexture;
    }

    backgroundSprite.setScale(
        1280.0f / std::max(menuBackgroundTexture.getSize().x, 1u),
        720.0f / std::max(menuBackgroundTexture.getSize().y, 1u)
    );
    
    messageText.setFont(font);
    messageText.setCharacterSize(20);
    messageText.setFillColor(sf::Color::White);
    messageText.setPosition(20.f, 680.f);
    
    inputText.setFont(font);
    inputText.setCharacterSize(22);
    inputText.setFillColor(sf::Color::White);
    
    playerNamesText.setFont(font);
    playerNamesText.setCharacterSize(18);
    playerNamesText.setFillColor(sf::Color::White);
}

void GUI::setupMainMenu() {
    buttons.clear();
    buttons.emplace_back(540, 500, 200, 50, font, "Start Game", [this]() {
        currentState = State::SelectPlayerCount;
        playerCount = 0;
        playerNames.clear();
        currentNameIndex = 0;
        updateButtons();
        showMessage("Select number of players (2-6)");
    });

    buttons.emplace_back(540, 560, 200, 50, font, "Exit", [this]() {
        window.close();
    });
}

void GUI::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else
                handleEvent(event);
        }

        float dt = clock.restart().asSeconds();
        update(dt);
        render();
    }
}

void GUI::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        // Handle popup first
        if (popup->isActive && popup->handleClick(mousePos)) {
            return;
        }
        
        // Handle block popup
        if (blockPopup->isActive && blockPopup->handleClick(mousePos)) {
            return;
        }
        
        // Handle spy panel
        if (spyPanel->isVisible && spyPanel->handleClick(mousePos)) {
            return;
        }
        
        // Handle regular buttons
        for (auto& btn : buttons) {
            if (btn.isClicked(mousePos)) {
                if (btn.onClick) btn.onClick();
                return;
            }
        }
        
        // Handle action buttons in playing state
        if (currentState == State::Playing) {
            for (auto& btn : actionButtons) {
                if (btn.isClicked(mousePos)) {
                    if (btn.onClick) btn.onClick();
                    return;
                }
            }
            
            // Check if player cards were clicked (for target selection)
            if (actionState == ActionState::SelectingTarget) {
                coup::Player* current = getCurrentPlayer();
                for (auto& card : playerCards) {
                    if (card.getBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                        coup::Player* target = card.getPlayer();
                        if (target && current && target != current) {
                            // Check if this is a valid target for the current action
                            std::vector<coup::Player*> validTargets = getValidTargets(current, pendingAction);
                            bool isValidTarget = std::find(validTargets.begin(), validTargets.end(), target) != validTargets.end();
                            
                            if (!isValidTarget) {
                                // Show appropriate error message based on action type
                                if (pendingAction == coup::ActionType::None && current->getRoleName() == "Spy" && !spyPanel->viewCoinsMode) {
                                    showErrorPopup(target->getName() + " is already blocked from using arrest!");
                                } else if (pendingAction == coup::ActionType::Arrest) {
                                    showErrorPopup(target->getName() + " cannot be arrested (no coins or already arrested)!");
                                } else if (pendingAction == coup::ActionType::Sanction) {
                                    showErrorPopup(target->getName() + " is already sanctioned!");
                                } else {
                                    showErrorPopup("Invalid target for this action!");
                                }
                                return;
                            }
                            
                            // Special handling for spy abilities to prevent turn ending
                            if (pendingAction == coup::ActionType::None && current->getRoleName() == "Spy") {
                                performAction(current, pendingAction, target);
                                // Don't change to WaitingForEndTurn - keep the turn active
                                actionState = ActionState::None;
                                refreshActionButtons();
                            } else {
                                performAction(current, pendingAction, target);
                                actionState = ActionState::WaitingForEndTurn;
                                refreshActionButtons();
                            }
                            return;
                        }
                    }
                }
            }
        }
        
        // Clear target selection if clicking elsewhere
        if (actionState == ActionState::SelectingTarget) {
            actionState = ActionState::None;
            showMessage("Action canceled");
            refreshActionButtons();
        }
    } 
    else if (event.type == sf::Event::TextEntered && currentState == State::EnterAllPlayerNames) {
        // Safety checks before handling text input
        if (currentNameIndex < 0 || static_cast<size_t>(currentNameIndex) >= playerNames.size()) {
            currentNameIndex = 0; // Reset if there's an issue
            currentInput = "";
            return;
        }

        // Filter out non-ASCII characters
        if (event.text.unicode == '\b') {  // Backspace
            if (!currentInput.empty()) {
                currentInput.pop_back();
            }
        } 
        else if (event.text.unicode == '\r' || event.text.unicode == '\n') {  // Enter
            if (!currentInput.empty()) {
                // Sanitize input to ensure it's safe
                std::string sanitizedInput = sanitizeInput(currentInput);
                
                if (sanitizedInput.empty()) {
                    showErrorPopup("Invalid characters in name. Please use only English letters, numbers, and basic symbols.");
                    return;
                }
                
                // Check for duplicate names
                bool isDuplicate = false;
                for (int i = 0; i < currentNameIndex; i++) {
                    if (playerNames[i] == sanitizedInput) {
                        isDuplicate = true;
                        break;
                    }
                }
                
                if (isDuplicate) {
                    showErrorPopup("Name '" + sanitizedInput + "' already exists. Please choose a different name.");
                } else {
                    playerNames[currentNameIndex] = sanitizedInput;
                    currentNameIndex++;
                    currentInput = "";
                    
                    if (currentNameIndex >= playerCount) {
                        startGame();
                    }
                }
            }
        } 
        else if (event.text.unicode < 128 && event.text.unicode >= 32) {  // ASCII printable chars only
            // Limit name length to 20 characters
            if (currentInput.length() < 20) {
                currentInput += static_cast<char>(event.text.unicode);
            }
        }
        
        // Update the text
        inputText.setString(currentInput + "_");
        
        // Update player names list
        std::string namesStr;
        for (int i = 0; i < playerCount; i++) {
            if (i < currentNameIndex && static_cast<size_t>(i) < playerNames.size()) {
                namesStr += std::to_string(i+1) + ": " + playerNames[i] + "\n";
            } else if (i == currentNameIndex) {
                namesStr += std::to_string(i+1) + ": [Entering...]\n";
            } else {
                namesStr += std::to_string(i+1) + ": ...\n";
            }
        }
        playerNamesText.setString(namesStr);
    }
}

void GUI::update(float dt) {
    if (messageTimer > 0.0f) {
        messageTimer -= dt;
        if (messageTimer <= 0.0f) {
            messageText.setString("");
        }
    }
    
    // Update popups
    popup->update(dt);
    
    // Update arrest blocks
    updateArrestBlocks();
    
    // Check for game over condition FIRST - to avoid crashes
    if (game && currentState == State::Playing) {
        try {
            std::vector<coup::Player*> activePlayers = game->getAllAlivePlayers();
            if (activePlayers.size() <= 1) {
                std::cout << "Game over detected in update - transitioning to GameOver state" << std::endl;
                currentState = State::GameOver;
                actionState = ActionState::None;
                hasPerformedAction = false;
                startedCurrentTurn = false;
                actionButtons.clear();
                updateButtons();
                // Make sure we don't proceed with turn logic after game over
                return;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error checking game state: " << e.what() << std::endl;
            currentState = State::GameOver;
            actionState = ActionState::None;
            hasPerformedAction = false;
            startedCurrentTurn = false;
            actionButtons.clear();
            updateButtons();
            return;
        }
    }
    
    // Update player cards
    if (currentState == State::Playing && game) {
        updatePlayerCards();
    }
    
    // Check for turn change - תיקון בעיית startTurn כפול
    if (game && currentState == State::Playing) {
        try {
            std::string currentPlayerName = game->turn();
            
            if (currentPlayerName != lastPlayerName) {
                std::cout << "Turn changed from '" << lastPlayerName << "' to '" << currentPlayerName << "'" << std::endl;
                
                // Reset flags for new turn
                lastPlayerName = currentPlayerName;
                hasPerformedAction = false;
                // לא מאפסים את startedCurrentTurn כאן כדי למנוע התחלת תור כפולה
                
                // עדכון כרטיסי שחקנים
                updatePlayerCards();
                
                // הכרחי להציג את הכפתורים כשהשחקן משתנה
                refreshActionButtons();
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in turn change: " << e.what() << std::endl;
            // במקרה של שגיאה, עבור למסך סיום
            currentState = State::GameOver;
            actionState = ActionState::None;
            hasPerformedAction = false;
            startedCurrentTurn = false;
            actionButtons.clear();
            updateButtons();
        }
    }
}

void GUI::updatePlayerCards() {
    if (!game) return;
    
    playerCards.clear();
    try {
        std::vector<coup::Player*> players = game->getAllAlivePlayers();
        
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i]) {
                PlayerCard card(players[i], font);
                float x = 20.f + (i % 3) * 270.f;
                float y = 80.f + (i / 3) * 140.f;
                card.setPosition(x, y);
                playerCards.push_back(card);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error updating player cards: " << e.what() << std::endl;
    }
}

void GUI::render() {
    window.clear(sf::Color::Black);

    // Draw background - use right texture for each state
    if (currentState == State::MainMenu) {
        backgroundSprite.setTexture(menuBackgroundTexture);
    } else if (currentState == State::GameOver) {
        backgroundSprite.setTexture(winnerBackgroundTexture);
    } else {
        backgroundSprite.setTexture(gameBackgroundTexture);
    }
    window.draw(backgroundSprite);

    // Draw appropriate UI for current state
    switch (currentState) {
        case State::MainMenu:
            drawMainMenu();
            break;
        case State::SelectPlayerCount:
            drawSelectPlayerCount();
            break;
        case State::EnterAllPlayerNames:
            drawEnterAllPlayerNames();
            break;
        case State::Playing:
            drawPlaying();
            break;
        case State::TargetSelection:
            drawTargetSelection();
            break;
        case State::GameOver:
            drawGameOver();
            break;
    }

    // Draw buttons
    for (auto& btn : buttons) {
        btn.draw(window);
    }
    
    // Draw action buttons in playing state
    if (currentState == State::Playing) {
        // עבור מצב משחק פעיל, וודא שתמיד יש כפתורי פעולה
        if (actionButtons.empty() && game && !game->getAllAlivePlayers().empty()) {
            refreshActionButtons();
        }
        
        for (auto& btn : actionButtons) {
            btn.draw(window);
        }
        
        // Draw special abilities section header if current player is spy
        coup::Player* currentPlayer = getCurrentPlayer();
        if (currentPlayer && currentPlayer->getRoleName() == "Spy") {
            sf::Text specialHeader("Special Abilities:", font, 16);
            specialHeader.setFillColor(sf::Color::Cyan);
            specialHeader.setPosition(1050.f, 170.f);
            window.draw(specialHeader);
            
            // Draw spy panel if visible
            spyPanel->draw(window);
        }
    }

    // Draw message if active
    if (messageTimer > 0.0f) {
        window.draw(messageText);
    }
    
    // Draw popup
    popup->draw(window);
    
    // Draw block popup
    blockPopup->draw(window);

    window.display();
}

void GUI::drawMainMenu() {
    // No titles, the background image should handle this
}

void GUI::drawSelectPlayerCount() {
    sf::Text title("Select Number of Players", font, 36);
    title.setFillColor(sf::Color::White);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition((window.getSize().x - bounds.width) / 2 - bounds.left, 100);
    window.draw(title);

    buttons.clear();
    float xStart = 440, yPos = 300, buttonSize = 60, spacing = 80;

    for (int i = 2; i <= 6; ++i) {
        buttons.emplace_back(xStart + (i - 2) * spacing, yPos, buttonSize, buttonSize, font, std::to_string(i), [this, i]() {
            playerCount = i;
            // Create name array of appropriate size
            playerNames.resize(playerCount);
            currentNameIndex = 0;
            currentInput = "";
            inputFocus = true;
            currentState = State::EnterAllPlayerNames;
            
            // Update player names text
            std::string namesStr;
            for (int j = 0; j < playerCount; j++) {
                if (j == 0) {
                    namesStr += std::to_string(j+1) + ": [Entering...]\n";
                } else {
                    namesStr += std::to_string(j+1) + ": ...\n";
                }
            }
            playerNamesText.setString(namesStr);
            
            updateButtons();
            showMessage("Enter player names");
        });
    }
    
    // Back button
    buttons.emplace_back(50, 650, 100, 40, font, "Back", [this]() {
        currentState = State::MainMenu;
        setupMainMenu();
    });
}

void GUI::drawEnterAllPlayerNames() {
    // Make sure name array matches required count
    if (static_cast<size_t>(playerCount) > playerNames.size()) {
        playerNames.resize(playerCount);
    }

    sf::Text title("Enter Player Names", font, 36);
    title.setFillColor(sf::Color::White);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition((window.getSize().x - bounds.width) / 2 - bounds.left, 100);
    window.draw(title);

    // Player list
    playerNamesText.setPosition(500.f, 180.f);
    window.draw(playerNamesText);

    // Current prompt
    sf::Text prompt("Enter Player " + std::to_string(currentNameIndex + 1) + " Name:", font, 24);
    prompt.setFillColor(sf::Color::Yellow);
    bounds = prompt.getLocalBounds();
    prompt.setPosition((window.getSize().x - bounds.width) / 2 - bounds.left, 400);
    window.draw(prompt);
    
    // Input box
    sf::RectangleShape inputBox(sf::Vector2f(400.f, 40.f));
    inputBox.setPosition(440.f, 450.f);
    inputBox.setFillColor(sf::Color(60, 60, 60));
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(sf::Color::White);
    window.draw(inputBox);

    inputText.setPosition(450.f, 455.f);
    window.draw(inputText);

    buttons.clear();
    
    // Next/Complete button
    if (!currentInput.empty()) {
        buttons.emplace_back(590, 510, 100, 40, font, 
            (currentNameIndex + 1 < playerCount) ? "Next" : "Start Game", [this]() {
            // Check for duplicate names
            bool isDuplicate = false;
            for (int i = 0; i < currentNameIndex; i++) {
                if (playerNames[i] == currentInput) {
                    isDuplicate = true;
                    break;
                }
            }
            
            if (isDuplicate) {
                showErrorPopup("Name '" + currentInput + "' already exists. Please choose a different name.");
            } else {
                playerNames[currentNameIndex] = sanitizeInput(currentInput);
                currentNameIndex++;
                currentInput = "";
                
                if (currentNameIndex >= playerCount) {
                    startGame();
                } else {
                    // Update player names text
                    std::string namesStr;
                    for (int i = 0; i < playerCount; i++) {
                        if (i < currentNameIndex) {
                            namesStr += std::to_string(i+1) + ": " + playerNames[i] + "\n";
                        } else if (i == currentNameIndex) {
                            namesStr += std::to_string(i+1) + ": [Entering...]\n";
                        } else {
                            namesStr += std::to_string(i+1) + ": ...\n";
                        }
                    }
                    playerNamesText.setString(namesStr);
                }
            }
        });
    }
    
    // Back button
    buttons.emplace_back(50, 650, 100, 40, font, "Back", [this]() {
        currentState = State::SelectPlayerCount;
        updateButtons();
    });
}

void GUI::drawPlaying() {
    // וידוא שלא מציירים במצב סיום משחק
    if (currentState == State::GameOver) {
        return;
    }
    
    if (!game) {
        showErrorPopup("Game not initialized!");
        return;
    }
    
    try {
        // Game info header
        sf::Text gameInfo;
        gameInfo.setFont(font);
        gameInfo.setCharacterSize(24);
        gameInfo.setFillColor(sf::Color::White);
        gameInfo.setPosition(20.f, 20.f);
        std::string infoStr = "Current Turn: " + game->turn() + "    Bank: " + std::to_string(game->getBankCoins()) + " coins";
        gameInfo.setString(infoStr);
        window.draw(gameInfo);
        
        // Draw player cards
        coup::Player* currentPlayer = getCurrentPlayer();
        for (auto& card : playerCards) {
            bool isCurrentPlayer = (card.getPlayer() == currentPlayer);
            card.draw(window, isCurrentPlayer);
        }
        
        // Overlay message if selecting target
        if (actionState == ActionState::SelectingTarget) {
            sf::Text targetMsg("Click on a player to select as target", font, 24);
            targetMsg.setFillColor(sf::Color::Yellow);
            sf::FloatRect bounds = targetMsg.getLocalBounds();
            targetMsg.setPosition((window.getSize().x - bounds.width) / 2, 500);
            window.draw(targetMsg);
            
            // Cancel button
            buttons.clear();
            buttons.emplace_back(580, 540, 120, 40, font, "Cancel", [this]() {
                actionState = ActionState::None;
                showMessage("Action canceled");
                refreshActionButtons();
            });
        } else {
            buttons.clear();
            buttons.emplace_back(1150, 20, 100, 40, font, "Quit", [this]() {
                // ניקוי משאבים לפני חזרה לתפריט הראשי
                if (game) {
                    std::cout << "Cleaning up game resources on quit" << std::endl;
                    delete game;
                    game = nullptr;
                    playerCards.clear();
                    actionButtons.clear();
                    arrestBlockedPlayers.clear();
                }
                currentState = State::MainMenu;
                setupMainMenu();
            });
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in drawPlaying: " << e.what() << std::endl;
        showErrorPopup("An error occurred in game display");
    } catch (...) {
        std::cerr << "Unknown error in drawPlaying" << std::endl;
        showErrorPopup("An unexpected error occurred");
    }
}

void GUI::drawTargetSelection() {
    // This is handled within drawPlaying when actionState is SelectingTarget
}

void GUI::drawGameOver() {
    buttons.clear();
    
    sf::Text title("Game Over", font, 64);
    title.setFillColor(sf::Color::White);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setPosition((window.getSize().x - bounds.width) / 2 - bounds.left, 150);
    window.draw(title);

    // Use cached winner name instead of repeatedly querying the game
    std::string winnerName = cachedWinnerName.empty() ? "Unknown" : cachedWinnerName;

    sf::Text winnerText("Winner: " + winnerName, font, 36);
    winnerText.setFillColor(sf::Color::Yellow);
    bounds = winnerText.getLocalBounds();
    winnerText.setPosition((window.getSize().x - bounds.width) / 2 - bounds.left, 250);
    window.draw(winnerText);

    buttons.emplace_back(540, 400, 200, 50, font, "Play Again", [this]() {
        // ניקוי משאבים לפני חזרה לתפריט הראשי
        if (game) {
            std::cout << "Cleaning up game resources on play again" << std::endl;
            delete game;
            game = nullptr;
            playerCards.clear();
            actionButtons.clear();
            arrestBlockedPlayers.clear();
        }
        cachedWinnerName = ""; // ניקוי שם המנצח השמור
        currentState = State::MainMenu;
        setupMainMenu();
    });

    buttons.emplace_back(540, 480, 200, 50, font, "Exit", [this]() {
        window.close();
    });
}

void GUI::startGame() {
    try {
        // ניקוי משאבים לפני יצירת משחק חדש
        if (game) {
            std::cout << "Cleaning up game resources before new game" << std::endl;
            delete game;
            game = nullptr;
            playerCards.clear();
            actionButtons.clear();
        }
        
        // Clear tracked state
        arrestBlockedPlayers.clear();
        
        // Create new game instance
        game = new coup::Game();
        game->setConsoleMode(false);
        
        // Create random players with given names
        for (int i = 0; i < playerCount; ++i) {
            coup::randomPlayer(*game, playerNames[i]);
        }
        
        // Initialize variables
        currentState = State::Playing;
        actionState = ActionState::None;
        hasPerformedAction = false;
        startedCurrentTurn = false; // Reset so startTurn will be called on first refresh
        lastPlayerName = "";
        cachedWinnerName = ""; // ניקוי שם המנצח השמור
        
        // וודא שיופיעו כפתורי פעולה מההתחלה
        updateButtons();
        updatePlayerCards();
        refreshActionButtons();  // וודא שקריאה זו נשארת
        
        showMessage("Game started! " + game->turn() + "'s turn.");
    } catch (const std::exception& e) {
        showErrorPopup("Failed to start game: " + std::string(e.what()));
        currentState = State::MainMenu;
        setupMainMenu();
    }
}

coup::Player* GUI::getCurrentPlayer() {
    if (!game) return nullptr;
    
    try {
        std::string currentName = game->turn();
        for (auto* p : game->getAllAlivePlayers()) {
            if (p && p->getName() == currentName) {
                return p;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in getCurrentPlayer: " << e.what() << std::endl;
    }
    
    return nullptr;
}

std::vector<coup::Player*> GUI::getValidTargets(coup::Player* current, coup::ActionType action) {
    std::vector<coup::Player*> validTargets;
    if (!current || !game) return validTargets;
    
    try {
        for (auto* p : game->getAllAlivePlayers()) {
            if (p && p != current) {
                // Filter targets based on action type
                if (action == coup::ActionType::Arrest) {
                    if (p->getArrestStatus() == coup::ArrestStatus::Available && p->getCoins() > 0) {
                        validTargets.push_back(p);
                    }
                } 
                else if (action == coup::ActionType::Sanction) {
                    if (!p->isSanctioned()) {
                        validTargets.push_back(p);
                    }
                }
                // Special case for Spy block arrest ability
                else if (action == coup::ActionType::None && current->getRoleName() == "Spy" && !spyPanel->viewCoinsMode) {
                    // Only allow targeting players who are NOT already arrest blocked
                    if (!p->isArrestBlocked() && !isPlayerArrestBlocked(p->getName())) {
                        validTargets.push_back(p);
                    }
                }
                else {
                    validTargets.push_back(p);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in getValidTargets: " << e.what() << std::endl;
    }
    
    return validTargets;
}

void GUI::refreshActionButtons() {
    actionButtons.clear();
    coup::Player* currentPlayer = getCurrentPlayer();
    
    if (!currentPlayer || !game) return;
    
    // קריאה ל-startTurn רק אם לא התחלנו כבר את התור
    if (!hasPerformedAction && !startedCurrentTurn && actionState == ActionState::None) {
        try {
            std::cout << "Starting turn for " << currentPlayer->getName() 
                      << " (" << currentPlayer->getRoleName() << ")" << std::endl;
            currentPlayer->startTurn();
            startedCurrentTurn = true;
        } catch (const std::exception& e) {
            std::cerr << "Error in startTurn: " << e.what() << std::endl;
        }
    }
    
    // If player has already performed an action this turn, only show end turn button
    // and possibly bribe if they have enough coins
    if (hasPerformedAction || actionState == ActionState::WaitingForEndTurn) {
        float buttonY = 600.f;
        float buttonWidth = 120.f;
        float buttonHeight = 40.f;
        float startX = 580.f;
        
        // End turn button
        actionButtons.emplace_back(startX, buttonY, buttonWidth, buttonHeight, font, "End Turn", [this, currentPlayer]() {
            try {
                currentPlayer->endTurn();
                showMessage(currentPlayer->getName() + " ended their turn");
                hasPerformedAction = false;
                startedCurrentTurn = false;  // Reset for the next turn
                lastPlayerName = "";  // Reset to force turn change detection
                actionState = ActionState::None;
                refreshActionButtons();
                updatePlayerCards();
            } catch (const std::exception& e) {
                showErrorPopup(e.what());
            }
        });
        
        // Bribe button if player has enough coins and hasn't used bribe yet
        if (currentPlayer->getCoins() >= 4 && !hasUsedBribe(currentPlayer)) {
            actionButtons.emplace_back(startX - 180.f, buttonY, buttonWidth, buttonHeight, font, "Bribe (4 Coins)", [this, currentPlayer]() {
                try {
                    performAction(currentPlayer, coup::ActionType::Bribe);
                    showMessage(currentPlayer->getName() + " used bribe - 4 coins paid");
                    hasPerformedAction = false; // Bribe allows another action
                    actionState = ActionState::None;
                    refreshActionButtons();
                    updatePlayerCards();
                } catch (const std::exception& e) {
                    showErrorPopup(e.what());
                }
            });
        }
        
        return;
    }
    
    // If player has 10+ coins, only allow coup
    if (currentPlayer->getCoins() >= 10) {
        float buttonY = 600.f;
        float buttonWidth = 120.f;
        float buttonHeight = 40.f;
        
        actionButtons.emplace_back(580, buttonY, buttonWidth, buttonHeight, font, "Coup (MUST)", [this, currentPlayer]() {
            std::vector<coup::Player*> validTargets = getValidTargets(currentPlayer, coup::ActionType::Coup);
            if (validTargets.empty()) {
                showErrorPopup("No valid targets for coup");
                return;
            }
            pendingAction = coup::ActionType::Coup;
            actionState = ActionState::SelectingTarget;
            showMessage("Select a player to coup (required with 10+ coins)");
            refreshActionButtons();
        });
        
        return;
    }
    
    // Regular action buttons when player hasn't performed an action yet
    float buttonY = 600.f;
    float buttonWidth = 120.f;
    float buttonHeight = 40.f;
    float spacing = 10.f;
    float startX = 20.f;
    
    // Core actions
    actionButtons.emplace_back(startX, buttonY, buttonWidth, buttonHeight, font, "Gather", [this, currentPlayer]() {
        try {
            currentPlayer->gather();
            showMessage(currentPlayer->getName() + " gathered coins");
            hasPerformedAction = true;
            actionState = ActionState::WaitingForEndTurn;
            refreshActionButtons();
            updatePlayerCards();
        } catch (const std::exception& e) {
            showErrorPopup(e.what());
        }
    });
    actionButtons.back().setEnabled(!currentPlayer->isSanctioned());
    
    // Tax button with check for blockers
    actionButtons.emplace_back(startX + buttonWidth + spacing, buttonY, buttonWidth, buttonHeight, font, "Tax", [this, currentPlayer]() {
        try {
            // Check if there's a player who can block tax
            if (checkForBlockers(currentPlayer, coup::ActionType::Tax)) {
                // Tax was blocked, do nothing
                return;
            }
            
            // If not blocked, proceed with the tax action
            currentPlayer->tax();
            showMessage(currentPlayer->getName() + " collected tax");
            hasPerformedAction = true;
            actionState = ActionState::WaitingForEndTurn;
            refreshActionButtons();
            updatePlayerCards();
        } catch (const std::exception& e) {
            showErrorPopup(e.what());
        }
    });
    actionButtons.back().setEnabled(!currentPlayer->isSanctioned());
    
    // Target actions
    actionButtons.emplace_back(startX + (buttonWidth + spacing) * 2, buttonY, buttonWidth, buttonHeight, font, "Arrest", [this, currentPlayer]() {
        std::vector<coup::Player*> validTargets = getValidTargets(currentPlayer, coup::ActionType::Arrest);
        if (validTargets.empty()) {
            showErrorPopup("No valid targets for arrest");
            return;
        }
        pendingAction = coup::ActionType::Arrest;
        actionState = ActionState::SelectingTarget;
        showMessage("Select a player to arrest");
        refreshActionButtons();
    });
    actionButtons.back().setEnabled(!currentPlayer->isArrestBlocked() && currentPlayer->getCoins() >= 1);
    
    actionButtons.emplace_back(startX + (buttonWidth + spacing) * 3, buttonY, buttonWidth, buttonHeight, font, "Sanction", [this, currentPlayer]() {
        std::vector<coup::Player*> validTargets = getValidTargets(currentPlayer, coup::ActionType::Sanction);
        if (validTargets.empty()) {
            showErrorPopup("No valid targets for sanction");
            return;
        }
        pendingAction = coup::ActionType::Sanction;
        actionState = ActionState::SelectingTarget;
        showMessage("Select a player to sanction");
        refreshActionButtons();
    });
    
    int sanctionCost = 3;
    if (currentPlayer->getRoleName() == "Judge") {
        // Judge pays extra cost
        sanctionCost = 4;
    }
    actionButtons.back().setEnabled(currentPlayer->getCoins() >= sanctionCost);
    
    actionButtons.emplace_back(startX + (buttonWidth + spacing) * 4, buttonY, buttonWidth, buttonHeight, font, "Coup", [this, currentPlayer]() {
        std::vector<coup::Player*> validTargets = getValidTargets(currentPlayer, coup::ActionType::Coup);
        if (validTargets.empty()) {
            showErrorPopup("No valid targets for coup");
            return;
        }
        pendingAction = coup::ActionType::Coup;
        actionState = ActionState::SelectingTarget;
        showMessage("Select a player to coup");
        refreshActionButtons();
    });
    actionButtons.back().setEnabled(currentPlayer->getCoins() >= 7);
    
    // Special actions based on role
    std::string specialActionName = "No Special";
    bool hasSpecialAction = false;
    std::string roleName = currentPlayer->getRoleName();
    
    // Special title for special abilities
    sf::Text specialTitle("Special:", font, 16);
    specialTitle.setFillColor(sf::Color::Yellow);
    specialTitle.setPosition(startX + (buttonWidth + spacing) * 5, buttonY - 25);
    
    if (roleName == "Baron" && currentPlayer->getCoins() >= 3) {
        specialActionName = "Invest";
        hasSpecialAction = true;
        actionButtons.emplace_back(startX + (buttonWidth + spacing) * 5, buttonY, buttonWidth, buttonHeight, font, specialActionName, [this, currentPlayer]() {
            try {
                // Baron special action
                if (coup::Baron* baron = dynamic_cast<coup::Baron*>(currentPlayer)) {
                    baron->invest();
                    showMessage(currentPlayer->getName() + " invested coins");
                    hasPerformedAction = true;
                    actionState = ActionState::WaitingForEndTurn;
                    refreshActionButtons();
                    updatePlayerCards();
                } else {
                    showErrorPopup("Player is not a Baron");
                }
            } catch (const std::exception& e) {
                showErrorPopup(e.what());
            }
        });
    } 
    else if (roleName == "Spy") {
        specialActionName = "Spy Abilities";
        hasSpecialAction = true;
        actionButtons.emplace_back(startX + (buttonWidth + spacing) * 5, buttonY, buttonWidth, buttonHeight, font, specialActionName, [this, currentPlayer]() {
            // Show spy panel with abilities
            spyPanel->show([this, currentPlayer](bool isViewCoins) {
                if (isViewCoins) {
                    // View coins mode
                    spyPanel->viewCoinsMode = true;
                    pendingAction = coup::ActionType::None; // Special action
                    actionState = ActionState::SelectingTarget;
                    showMessage("Select a player to view their coins");
                } else {
                    // Block arrest mode
                    spyPanel->viewCoinsMode = false;
                    pendingAction = coup::ActionType::None; // Special action 
                    actionState = ActionState::SelectingTarget;
                    showMessage("Select a player to block their arrest ability");
                }
            });
        });
    }
    
    if (!hasSpecialAction) {
        // Disabled special action button for other roles
        actionButtons.emplace_back(startX + (buttonWidth + spacing) * 5, buttonY, buttonWidth, buttonHeight, font, "No Special", [this]() {
            showMessage("No special action available for this role");
        });
        actionButtons.back().setEnabled(false);
    }
    
    // Skip turn button - always available
    actionButtons.emplace_back(startX + (buttonWidth + spacing) * 6, buttonY, buttonWidth, buttonHeight, font, "Skip Turn", [this, currentPlayer]() {
        currentPlayer->endTurn();
        showMessage(currentPlayer->getName() + " skipped their turn");
        hasPerformedAction = false;
        startedCurrentTurn = false;  // Reset for next turn
        lastPlayerName = "";  // Reset to force turn change detection
        actionState = ActionState::None;
        refreshActionButtons();
        updatePlayerCards();
    });
}

void GUI::performAction(coup::Player* player, coup::ActionType action, coup::Player* target) {
    if (!player || !game) {
        showErrorPopup("No active player or game");
        return;
    }
    
    try {
        // Check for game over condition before performing action
        if (currentState == State::GameOver) {
            showMessage("Game has already ended");
            return;
        }
        
        // Check for blockers for actions that can be blocked (bribe, tax)
        if ((action == coup::ActionType::Bribe || action == coup::ActionType::Tax) && 
            checkForBlockers(player, action, target)) {
            // Action was blocked, do nothing
            return;
        }
        
        if (action == coup::ActionType::None && player->getRoleName() == "Spy" && target) {
            // Special case for spy abilities
            if (coup::Spy* spy = dynamic_cast<coup::Spy*>(player)) {
                if (spyPanel->viewCoinsMode) {
                    // View coins
                    int targetCoins = spy->peekCoins(*target);
                    showInfoPopup(target->getName() + " has " + std::to_string(targetCoins) + " coins");
                    // Spy abilities don't end the turn - player can continue
                    actionState = ActionState::None;
                    refreshActionButtons();
                } else {
                    // Block arrest - check if target is already blocked
                    if (target->isArrestBlocked() || isPlayerArrestBlocked(target->getName())) {
                        showErrorPopup(target->getName() + " is already blocked from using arrest!");
                        actionState = ActionState::None;
                        refreshActionButtons();
                        return;
                    }
                    
                    // Block arrest - store in our tracking map
                    spy->blockNextArrest(*target);
                    // Track this in our map to help visualize it
                    arrestBlockedPlayers[target->getName()] = 3; // Block for 3 turns for reliability
                    showMessage(player->getName() + " blocked " + target->getName() + "'s arrest ability");
                    // Spy abilities don't end the turn - player can continue
                    actionState = ActionState::None;
                    refreshActionButtons();
                }
                updatePlayerCards();
                return;
            }
        }
        
        switch (action) {
            case coup::ActionType::Gather:
                player->gather();
                showMessage(player->getName() + " gathered coins");
                break;
                
            case coup::ActionType::Tax:
                player->tax();
                showMessage(player->getName() + " collected tax");
                break;
                
            case coup::ActionType::Arrest:
                if (target) {
                    if (player->getCoins() < 1) {
                        showErrorPopup("Not enough coins to arrest!");
                        return;
                    }
                    player->arrest(*target);
                    showMessage(player->getName() + " arrested " + target->getName());
                } else {
                    showErrorPopup("No target selected for arrest");
                    return;
                }
                break;
                
            case coup::ActionType::Sanction:
                if (target) {
                    int sanctionCost = 3;
                    if (player->getRoleName() == "Judge") {
                        sanctionCost = 4;
                    }
                    if (player->getCoins() < sanctionCost) {
                        showErrorPopup("Not enough coins to sanction!");
                        return;
                    }
                    player->sanction(*target);
                    showMessage(player->getName() + " sanctioned " + target->getName());
                } else {
                    showErrorPopup("No target selected for sanction");
                    return;
                }
                break;
                
            case coup::ActionType::Coup:
                if (target) {
                    // Check if enough coins
                    if (player->getCoins() < 7) {
                        showErrorPopup("Not enough coins to coup!");
                        return;
                    }
                    
                    // שמירת שם השחקן לפני הcoup למקרה שהוא המנצח
                    std::string potentialWinner = player->getName();
                    std::string targetName = target->getName(); // שמירת שם היעד לפני שהוא נמחק
                    
                    try {
                        // ביצוע הcoup
                        player->coup(*target);
                        showMessage(player->getName() + " couped " + targetName);
                        
                        // שחרור מיד את השימוש במצביע לשחקן המודח - חשוב מאוד!
                        target = nullptr;
                        
                        // עדכון מיידי של כרטיסי השחקנים להסרת מצביעים לא תקפים
                        updatePlayerCards();
                        
                        // בדיקה מיידית של סיום המשחק
                        try {
                            std::vector<coup::Player*> remainingPlayers = game->getAllAlivePlayers();
                            std::cout << "Players remaining after coup: " << remainingPlayers.size() << std::endl;
                            
                            if (remainingPlayers.size() <= 1) {
                                std::cout << "Game over after coup - transitioning to GameOver state" << std::endl;
                                
                                // שמירת המנצח לפני מעבר למסך סיום
                                if (remainingPlayers.size() == 1) {
                                    cachedWinnerName = remainingPlayers[0]->getName();
                                } else {
                                    cachedWinnerName = potentialWinner;
                                }
                                std::cout << "Winner name cached: " << cachedWinnerName << std::endl;
                                
                                // מעבר בטוח למסך סיום מיד
                                currentState = State::GameOver;
                                actionState = ActionState::None;
                                pendingAction = coup::ActionType::None;
                                hasPerformedAction = false;
                                startedCurrentTurn = false;
                                
                                // ניקוי כפתורי פעולה למניעת גישה לשחקנים מתים
                                actionButtons.clear();
                                
                                // עדכון מיידי של כרטיסי השחקנים
                                updatePlayerCards();
                                updateButtons();
                                
                                // חשוב: יציאה מיידית מהפונקציה למניעת גישה לזיכרון לא תקין
                                return;
                            }
                        } catch (std::exception& e) {
                            // במקרה של שגיאה, מניחים שהמשחק הסתיים
                            std::cerr << "Error checking players after coup: " << e.what() << std::endl;
                            currentState = State::GameOver;
                            actionState = ActionState::None;
                            hasPerformedAction = false;
                            startedCurrentTurn = false;
                            actionButtons.clear();
                            updateButtons();
                            return;
                        } catch (...) {
                            std::cerr << "Unknown error checking players after coup" << std::endl;
                            currentState = State::GameOver;
                            actionState = ActionState::None;
                            hasPerformedAction = false;
                            startedCurrentTurn = false;
                            actionButtons.clear();
                            updateButtons();
                            return;
                        }
                    } catch (const std::exception& e) {
                        showErrorPopup(e.what());
                        return;
                    } catch (...) {
                        showErrorPopup("Unknown error during coup");
                        return;
                    }
                } else {
                    showErrorPopup("No target selected for coup");
                    return;
                }
                break;
                
            case coup::ActionType::Bribe:
                if (player->getCoins() < 4) {
                    showErrorPopup("Not enough coins to bribe!");
                    return;
                }
                player->bribe();
                showMessage(player->getName() + " paid 4 coins for bribe");
                break;
                
            case coup::ActionType::Invest:
                if (coup::Baron* baron = dynamic_cast<coup::Baron*>(player)) {
                    if (player->getCoins() < 3) {
                        showErrorPopup("Not enough coins to invest!");
                        return;
                    }
                    baron->invest();
                    showMessage(player->getName() + " invested coins");
                }
                break;
                
            case coup::ActionType::None:
                // Already handled above
                return;
        }
        
        hasPerformedAction = true;
        actionState = ActionState::WaitingForEndTurn;
        updatePlayerCards();
        refreshActionButtons();
    } catch (const std::exception& e) {
        showErrorPopup(e.what());
    }
}