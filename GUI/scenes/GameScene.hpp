// GUI/scenes/GameScene.hpp
#pragma once

#include "Scene.hpp"
#include "../components/Button.hpp"
#include "../components/Popup.hpp"
#include "../game/GameController.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace coup {
namespace gui {

class GameScene : public Scene {
private:
    // Resources
    sf::Sprite background;
    GameController* gameController;

    // UI Elements
    std::unique_ptr<Button> exitButton;
    std::unique_ptr<Popup> messagePopup;

    // Game info display
    sf::Text bankText;
    sf::Text turnText;
    sf::Text statusText;

    // Player cards
    struct PlayerCard {
        sf::RectangleShape background;
        sf::Text nameText;
        sf::Text roleText;
        sf::Text coinsText;
        sf::Text statusText;
        bool isActive = false;
        bool isAlive = true;
    };
    std::vector<PlayerCard> playerCards;

    // Action buttons
    std::unordered_map<std::string, std::unique_ptr<Button>> commonButtons;
    std::unordered_map<std::string, std::unique_ptr<Button>> specialButtons;

    // Turn control buttons
    std::unique_ptr<Button> startTurnButton;
    std::unique_ptr<Button> endTurnButton;
    std::unique_ptr<Button> bribeYesButton;
    std::unique_ptr<Button> bribeNoButton;

    // Target selection
    bool selectingTarget = false;
    std::string pendingAction;
    std::vector<std::unique_ptr<Button>> targetButtons;

    // Message display
    sf::Text messageText;
    float messageTimer = 0.0f;

public:
    GameScene(GameController* controller);

    void onEnter() override;
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float deltaTime, const sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;

private:
    void setupUI();
    void updateGameState();
    void updateButtons();
    void createPlayerCards();
    void createActionButtons();
    void showTargetSelection(const std::string& action);
    void hideTargetSelection();
    void showMessage(const std::string& msg, float duration = 3.0f);
    void handleForcedCoup();

    // Action handlers
    void onGather();
    void onTax();
    void onBribe();
    void onArrest(const std::string& target);
    void onSanction(const std::string& target);
    void onCoup(const std::string& target);
    void onInvest();
    void onPeek(const std::string& target);
    void onBlockArrest(const std::string& target);
    void onUndo();
};

} // namespace gui
} // namespace coup