// GUI/scenes/PlayerNamesScene.hpp
#pragma once

#include "Scene.hpp"
#include "../components/Button.hpp"
#include "../components/Popup.hpp"
#include "../game/GameController.hpp"
#include <memory>
#include <vector>
#include <string>

namespace coup {
namespace gui {

class PlayerNamesScene : public Scene {
private:
    sf::Sprite background;
    sf::Text titleText;
    sf::Text instructionText;

    // Input fields
    std::vector<sf::RectangleShape> inputBoxes;
    std::vector<sf::Text> inputTexts;
    std::vector<sf::Text> labels;
    std::vector<std::string> names;

    // Buttons
    std::unique_ptr<Button> startButton;
    std::unique_ptr<Button> backButton;
    std::unique_ptr<Button> exitButton;

    // Popup for errors
    std::unique_ptr<Popup> errorPopup;

    // State
    int playerCount = 0;
    int activeInput = 0;

    // Game controller
    GameController* gameController;

public:
    PlayerNamesScene(GameController* controller);

    void setPlayerCount(int count);
    void reset();

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void update(float deltaTime, const sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window) override;

private:
    void createInputFields();
    void handleTextInput(sf::Uint32 unicode);
    bool validateNames();
    void tryCreateGame();
};

} // namespace gui
} // namespace coup