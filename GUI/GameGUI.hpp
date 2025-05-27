// Email: nitzanwa@gmail.com

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "../GameLogic/Game.hpp"
#include "../Players/Player.hpp"

namespace coup {

// Simple Button class
class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> onClick;

    Button(float x, float y, float width, float height, const sf::Font& font,
           const std::string& label, std::function<void()> cb);
    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2i mousePos);
    void setEnabled(bool enabled);
};

// Simple popup window
class MessagePopup {
private:
    sf::RectangleShape overlay;
    sf::RectangleShape popup;
    sf::Text messageText;
    sf::Text buttonText;
    sf::RectangleShape button;
    bool isVisible;
    std::string currentMessage;

public:
    MessagePopup() : isVisible(false) {}  // Add default constructor
    MessagePopup(const sf::Font& font);
    void show(const std::string& message);
    void hide();
    bool handleClick(sf::Vector2i mousePos);
    void draw(sf::RenderWindow& window);
    bool visible() const { return isVisible; }
};

// Blocking decision dialog
class BlockingDialog {
private:
    sf::RectangleShape overlay;
    sf::RectangleShape dialog;
    sf::Text messageText;
    sf::RectangleShape yesButton;
    sf::RectangleShape noButton;
    sf::Text yesText;
    sf::Text noText;
    bool isVisible;
    std::function<void()> onYes;
    std::function<void()> onNo;

public:
    BlockingDialog() : isVisible(false) {}
    BlockingDialog(const sf::Font& font) : isVisible(false) {}
    void show(const std::string& message, std::function<void()> yesCallback, std::function<void()> noCallback) {}
    void hide() { isVisible = false; }
    bool handleClick(sf::Vector2i mousePos) { return false; }
    void draw(sf::RenderWindow& window) {}
    bool visible() const { return isVisible; }
};

// Main GUI class
class CoupGUI {
public:
    CoupGUI();
    ~CoupGUI();

    void run();

private:
    // SFML components
    sf::RenderWindow window;
    sf::Font font;
    sf::Texture menuBackgroundTexture;
    sf::Texture gameBackgroundTexture;
    sf::Sprite backgroundSprite;

    // UI components
    std::vector<Button> buttons;
    sf::Text messageText;
    std::string currentMessage;
    float messageTimer;
    static constexpr float MESSAGE_DURATION = 3.0f;
    MessagePopup popup;
    BlockingDialog blockingDialog;

    // Game state
    enum class State { Start, SelectCount, EnterNames, Playing, GameOver };
    State currentState = State::Start;

    // Blocking decision state
    enum class BlockingDecision { None, Pending, Yes, No };
    BlockingDecision currentBlockingDecision = BlockingDecision::None;
    bool blockingInProgress = false;

    // Game objects
    std::unique_ptr<Game> game;
    std::vector<Player*> players;

    // Player setup
    int playerCount = 0;
    std::vector<std::string> nameBuffers;
    std::vector<std::string> playerNames;
    int nameIndex = 0;
    bool playerCountSelected = false;

    // Turn management
    bool turnInProgress = false;
    bool waitingForBribeDecision = false;

    // Event handling
    void handleEvent(const sf::Event& event);
    void render(float deltaTime);

    // State drawing methods
    void drawStartMenu();
    void drawPlayerCountButtons();
    void drawNameInputs();
    void drawPlayers();
    void drawExitButton();

    // Game logic
    void startGame();
    void setupCallbacks();
    Player* getCurrentPlayer();
    void updateButtons();
    void checkForWinner();
    void resetGame();

    // Safe target collection to prevent segfault
    std::vector<Player*> getValidTargets(Player* current);

    // UI helpers
    void showMessage(const std::string& msg);
    void showError(const std::string& msg);
    void showPopup(const std::string& msg);
    void loadBackgrounds();

    // Name validation
    bool isDuplicateName(const std::string& name) const;
    // Helper to get action name
    std::string getActionName(ActionType action) const {
        switch (action) {
            case ActionType::Tax: return "Tax";
            case ActionType::Bribe: return "Bribe";
            case ActionType::Coup: return "Coup";
            case ActionType::Gather: return "Gather";
            case ActionType::Arrest: return "Arrest";
            case ActionType::Sanction: return "Sanction";
            default: return "Unknown";
        }
    }

    // Process blocking decision
    bool processBlockingDecision();
};

}