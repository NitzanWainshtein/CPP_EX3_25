#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <string>
#include "../GameLogic/Game.hpp"
#include "../Players/Player.hpp"

// Button class for GUI elements
class Button {
public:
    Button(float x, float y, float width, float height, 
           const sf::Font& font, const std::string& label, 
           std::function<void()> onClick);
    
    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2i mousePos);
    void setEnabled(bool enabled);
    void setSelected(bool selected);
    void setPosition(float x, float y);
    sf::Vector2f getSize() const;
    void setLabel(const std::string& newLabel);
    
    std::function<void()> onClick;
    
private:
    sf::RectangleShape shape;
    sf::Text text;
    bool enabled;
};

// Player card UI element
class PlayerCard {
public:
    PlayerCard(coup::Player* player, const sf::Font& font);
    
    void update();
    void draw(sf::RenderWindow& window, bool isCurrentPlayer);
    void setPosition(float x, float y);
    sf::FloatRect getBounds() const;
    
    coup::Player* getPlayer() const { return player; }
    
private:
    coup::Player* player;
    sf::RectangleShape background;
    sf::Text nameText;
    sf::Text roleText;
    sf::Text coinsText;
    sf::Text statusText;
};

// Message popup
class PopupMessage {
public:
    PopupMessage(const sf::Font& font);
    
    void show(const std::string& message, bool isError);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool handleClick(sf::Vector2i mousePos);
    
    bool isActive;
    
private:
    sf::RectangleShape background;
    sf::Text messageText;
    std::unique_ptr<Button> okButton;
    float timer;
};

// Blocking action popup
class BlockActionPopup {
public:
    BlockActionPopup(const sf::Font& font);
    
    void show(coup::Player* blocker, coup::Player* actor, 
              coup::ActionType action, coup::Player* target,
              std::function<void(bool)> callback);
    void draw(sf::RenderWindow& window);
    bool handleClick(sf::Vector2i mousePos);
    void hide() { isActive = false; };
    
    std::unique_ptr<Button> yesButton;
    std::unique_ptr<Button> noButton;
    sf::Text messageText;
    sf::Text titleText;
    bool isActive;
    
private:
    sf::RectangleShape background;
    coup::Player* blocker;
    coup::Player* actor;
    coup::ActionType action;
    coup::Player* target;
    std::function<void(bool)> callback;
};

// Spy abilities panel
class SpyAbilitiesPanel {
public:
    SpyAbilitiesPanel(const sf::Font& font);
    
    void show(std::function<void(bool)> spyActionCallback);
    void draw(sf::RenderWindow& window);
    bool handleClick(sf::Vector2i mousePos);
    void setPosition(float x, float y);
    
    bool isVisible;
    bool viewCoinsMode;
    
private:
    sf::RectangleShape background;
    sf::Text titleText;
    std::unique_ptr<Button> viewCoinsButton;
    std::unique_ptr<Button> blockArrestButton;
    std::unique_ptr<Button> closeButton;
};

// Main GUI class
class GUI {
public:
    GUI();
    ~GUI();
    
    void run();
    
private:
    // Game states
    enum class State {
        MainMenu,
        SelectPlayerCount,
        EnterAllPlayerNames,
        Playing,
        TargetSelection,
        GameOver
    };
    void cleanupGame();
    std::string cachedWinnerName;
    // Action states within game
    enum class ActionState {
        None,
        SelectingTarget,
        WaitingForEndTurn,
        WaitingForBlock
    };
    
    // Constants
    const float MESSAGE_DURATION = 3.0f;
    
    // State variables
    State currentState;
    ActionState actionState;
    coup::ActionType pendingAction;
    bool startedCurrentTurn;
    std::string lastPlayerName;
    
    // SFML objects
    sf::RenderWindow window;
    sf::Font font;
    sf::Texture menuBackgroundTexture;
    sf::Texture gameBackgroundTexture;
    sf::Texture winnerBackgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Text messageText;
    sf::Text inputText;
    sf::Text playerNamesText;
    
    // Game objects
    coup::Game* game;
    int playerCount;
    std::vector<std::string> playerNames;
    int currentNameIndex;
    std::string currentInput;
    bool inputFocus;
    bool hasPerformedAction;
    float messageTimer;
    
    // UI collections
    std::vector<Button> buttons;
    std::vector<Button> actionButtons;
    std::vector<PlayerCard> playerCards;
    
    // Special UI panels
    std::unique_ptr<PopupMessage> popup;
    std::unique_ptr<BlockActionPopup> blockPopup;
    std::unique_ptr<SpyAbilitiesPanel> spyPanel;
    
    // Tracked game state
    std::map<std::string, int> arrestBlockedPlayers;
    
    // Initialization
    void loadAssets();
    void setupMainMenu();
    void startGame();
    
    // Game logic
    void updateButtons();
    bool checkForBlockers(coup::Player* actor, coup::ActionType action, coup::Player* target = nullptr);
    coup::Player* findPotentialBlocker(coup::ActionType action);
    bool showYesNoPopup(const std::string& message);
    coup::Player* getCurrentPlayer();
    std::vector<coup::Player*> getValidTargets(coup::Player* current, coup::ActionType action);
    void refreshActionButtons();
    void updatePlayerCards();
    
    // Event handling
    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render();
    
    // Drawing functions
    void drawMainMenu();
    void drawSelectPlayerCount();
    void drawEnterAllPlayerNames();
    void drawPlaying();
    void drawTargetSelection();
    void drawGameOver();
    
    // Action handling
    void performAction(coup::Player* player, coup::ActionType action, coup::Player* target = nullptr);
    bool hasUsedBribe(coup::Player* player) const;
    bool isPlayerArrestBlocked(const std::string& playerName) const;
    void updateArrestBlocks();
    
    // Helper functions
    void showMessage(const std::string& msg);
    void showErrorPopup(const std::string& msg);
    void showInfoPopup(const std::string& msg);
    std::string sanitizeInput(const std::string& input);
    bool isValidUtf8(const std::string& str);
};