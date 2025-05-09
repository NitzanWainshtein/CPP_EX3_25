#include "GUI.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <optional>
#include <chrono>

#include "../GameLogic/Game.hpp"
#include "../GameLogic/PlayerFactory.hpp"
#include "../Players/Player.hpp"
#include "../Players/Roles/Spy.hpp"
#include "../Players/Roles/Merchant.hpp"

using namespace sf;
using namespace std;

namespace coup {
    enum class ScreenState {
        Welcome,
        ChooseCount,
        EnterNames,
        Summary,
        GameView,
        TargetPopup,
        MessagePopup
    };

    static ScreenState currentScreen = ScreenState::Welcome;
    static int participantCount = 2;
    static vector<string> playerNames;
    static vector<Text> nameTexts;
    static vector<Text> nameLabels;
    static vector<Text> summaryTexts;
    static vector<Text> gameViewTexts;
    static vector<Text> activePlayerList;

    static int activeInputIndex = 0;
    static Game game;
    static vector<Player *> createdPlayers;
    static string errorMessage = "";
    static string temporaryMessage = "";
    static chrono::steady_clock::time_point messageStartTime;

    // GameView state
    static Player *currentPlayer = nullptr;
    static string pendingAction = "";
    static bool bribeExtraMove = false;
    static vector<pair<RectangleShape, string> > abilityButtons;
    static vector<pair<RectangleShape, string> > specialButtons;

    // Target Popup
    static vector<Player *> validTargets;
    static size_t targetIndex = 0;
    static RectangleShape popupBox, confirmButton, cancelButton;
    static Text popupTitle, targetNameText, confirmText, cancelText, leftArrowText, rightArrowText;
    static RectangleShape leftArrow, rightArrow;

    // GUI elements
    static Font font;
    static Texture bgMenuTexture, bgGameTexture;
    static Sprite menuBackground, gameBackground;
    static RectangleShape terminateButton;
    static Text terminateText;

    // Declarations (for organization)
    void drawGameUI(RenderWindow &window);

    void handleActionClick(const std::string &actionName);

    void handleTargetPopupClick(sf::Vector2f mouse);

    void setupTargetPopup(const string &actionName);

    void drawTargetPopup(RenderWindow &window);

    void showTemporaryMessage(const string &message);

    void drawTemporaryMessage(RenderWindow &window);

    void showTemporaryMessage(const string &message) {
        temporaryMessage = message;
        messageStartTime = chrono::steady_clock::now();
    }

    void drawTemporaryMessage(RenderWindow &window) {
        if (temporaryMessage.empty()) return;
        auto now = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(now - messageStartTime).count();
        if (duration > 3) {
            temporaryMessage = "";
            return;
        }

        RectangleShape background(Vector2f(500, 60));
        background.setFillColor(Color(0, 0, 0, 180));
        background.setPosition(150, 260);
        window.draw(background);

        Text msgText(temporaryMessage, font, 22);
        msgText.setFillColor(Color::White);
        msgText.setPosition(170, 275);
        window.draw(msgText);
    }

    void setupInputFields(vector<RectangleShape> &inputBoxes) {
        playerNames = vector<string>(participantCount, "");
        nameTexts.clear();
        nameLabels.clear();
        inputBoxes.clear();

        for (int i = 0; i < participantCount; ++i) {
            RectangleShape box(Vector2f(400, 40));
            box.setPosition(200, 80 + i * 60);
            box.setFillColor(Color(255, 255, 255, 200));
            inputBoxes.push_back(box);

            Text text("", font, 24);
            text.setFillColor(Color::Black);
            text.setPosition(210, 85 + i * 60);
            nameTexts.push_back(text);

            Text label("Player " + to_string(i + 1) + ":", font, 22);
            label.setFillColor(Color::White);
            label.setPosition(100, 85 + i * 60);
            nameLabels.push_back(label);
        }
    }

    void setupSummary() {
        summaryTexts.clear();
        Text header("Players list:", font, 30);
        header.setFillColor(Color::White);
        header.setPosition(280, 20);
        summaryTexts.push_back(header);

        Text columns("NAME               ROLE", font, 24);
        columns.setFillColor(Color::Yellow);
        columns.setPosition(220, 70);
        summaryTexts.push_back(columns);

        for (size_t i = 0; i < createdPlayers.size(); ++i) {
            string line = playerNames[i] + string(20 - playerNames[i].length(), ' ') + createdPlayers[i]->getRoleName();
            Text t(line, font, 24);
            t.setFillColor(Color::White);
            t.setPosition(220, 110 + static_cast<int>(i) * 40);
            summaryTexts.push_back(t);
        }
    }

    void setupGameView() {
        gameViewTexts.clear();
        abilityButtons.clear();
        specialButtons.clear();
        activePlayerList.clear();
        currentPlayer = nullptr;
        pendingAction.clear();
        bribeExtraMove = false;

        for (auto *p: createdPlayers) {
            if (p && p->getName() == game.turn()) {
                currentPlayer = p;
                break;
            }
        }
        if (!currentPlayer) return;

        try {
            currentPlayer->startTurn();
        } catch (const std::exception &e) {
            pendingAction = "coup_only";
        }

        string info = "Turn: " + currentPlayer->getName() + " | Role: " + currentPlayer->getRoleName() + " | Coins: " +
                      to_string(currentPlayer->getCoins());
        Text infoText(info, font, 24);
        infoText.setFillColor(Color::Cyan);
        infoText.setPosition(160, 20);
        gameViewTexts.push_back(infoText);

        Text abTitle("Abilities:", font, 20);
        abTitle.setFillColor(Color::White);
        abTitle.setPosition(520, 60);
        gameViewTexts.push_back(abTitle);

        vector<string> baseActions;
        if (pendingAction == "coup_only") {
            baseActions = {"coup"};
        } else {
            baseActions = {"gather", "tax", "bribe", "arrest", "sanction", "coup"};
        }

        for (size_t i = 0; i < baseActions.size(); ++i) {
            RectangleShape btn(Vector2f(150, 35));
            btn.setPosition(520, 90 + i * 45);
            btn.setFillColor(Color::Blue);
            abilityButtons.push_back({btn, baseActions[i]});
        }

        vector<string> specials;
        if (currentPlayer->getRoleName() == "Spy") specials.push_back("peekCoins");

        if (!specials.empty()) {
            Text spTitle("Special Abilities:", font, 20);
            spTitle.setFillColor(Color::White);
            spTitle.setPosition(520, 370);
            gameViewTexts.push_back(spTitle);
            for (size_t i = 0; i < specials.size(); ++i) {
                RectangleShape btn(Vector2f(150, 35));
                btn.setPosition(520, 400 + i * 45);
                btn.setFillColor(Color::Magenta);
                specialButtons.push_back({btn, specials[i]});
            }
        }

        vector<string> alive = game.players();
        Text listLabel("Active Players:", font, 20);
        listLabel.setFillColor(Color::White);
        listLabel.setPosition(20, 520);
        activePlayerList.push_back(listLabel);
        for (size_t i = 0; i < alive.size(); ++i) {
            Text name(alive[i], font, 18);
            name.setFillColor(Color::Green);
            name.setPosition(160 + i * 100, 520);
            activePlayerList.push_back(name);
        }

        terminateButton = RectangleShape(Vector2f(140, 30));
        terminateButton.setPosition(640, 20);
        terminateButton.setFillColor(Color::Red);
        terminateText = Text("Terminate Game", font, 16);
        terminateText.setFillColor(Color::White);
        terminateText.setPosition(645, 23);
    }

    void setupTargetPopup(const string &actionName) {
        pendingAction = actionName;
        validTargets.clear();

        for (auto *p: createdPlayers) {
            if (p && p != currentPlayer) {
                if (game.players().end() != std::find(game.players().begin(), game.players().end(), p->getName()))
                    validTargets.push_back(p);
            }
        }

        if (validTargets.empty()) {
            pendingAction.clear();
            return;
        }

        currentScreen = ScreenState::TargetPopup;
        targetIndex = 0;

        popupBox = RectangleShape(Vector2f(500, 200));
        popupBox.setFillColor(Color(0, 0, 0, 220));
        popupBox.setOutlineColor(Color::White);
        popupBox.setOutlineThickness(2);
        popupBox.setPosition(150, 180);

        popupTitle = Text("Choose Target for " + actionName, font, 22);
        popupTitle.setFillColor(Color::White);
        popupTitle.setPosition(180, 200);

        leftArrowText = Text("<", font, 28);
        leftArrowText.setFillColor(Color::White);
        leftArrowText.setPosition(220, 260);

        rightArrowText = Text(">", font, 28);
        rightArrowText.setFillColor(Color::White);
        rightArrowText.setPosition(430, 260);

        targetNameText = Text("", font, 26);
        targetNameText.setFillColor(Color::Yellow);
        targetNameText.setPosition(270, 260);

        confirmButton = RectangleShape(Vector2f(120, 35));
        confirmButton.setFillColor(Color::Green);
        confirmButton.setPosition(210, 320);
        confirmText = Text("Confirm", font, 18);
        confirmText.setFillColor(Color::Black);
        confirmText.setPosition(230, 327);

        cancelButton = RectangleShape(Vector2f(120, 35));
        cancelButton.setFillColor(Color::Red);
        cancelButton.setPosition(360, 320);
        cancelText = Text("Cancel", font, 18);
        cancelText.setFillColor(Color::White);
        cancelText.setPosition(380, 327);
    }

    void drawTargetPopup(RenderWindow &window) {
        window.draw(popupBox);
        window.draw(popupTitle);

        if (!validTargets.empty()) {
            targetNameText.setString(validTargets[targetIndex]->getName());
            window.draw(leftArrowText);
            window.draw(rightArrowText);
            window.draw(targetNameText);
            window.draw(confirmButton);
            window.draw(confirmText);
            window.draw(cancelButton);
            window.draw(cancelText);
        }
    }

    void handleTargetPopupClick(Vector2f mouse) {
        if (leftArrowText.getGlobalBounds().contains(mouse) && targetIndex > 0)
            targetIndex--;
        else if (rightArrowText.getGlobalBounds().contains(mouse) && targetIndex < validTargets.size() - 1)
            targetIndex++;
        else if (cancelButton.getGlobalBounds().contains(mouse)) {
            currentScreen = ScreenState::GameView;
            pendingAction.clear();
        } else if (confirmButton.getGlobalBounds().contains(mouse)) {
            try {
                if (pendingAction == "peekCoins") {
                    auto *spy = dynamic_cast<Spy *>(currentPlayer);
                    if (spy) {
                        int c = spy->peekCoins(*validTargets[targetIndex]);
                        showTemporaryMessage(validTargets[targetIndex]->getName() + " has " + to_string(c) + " coins.");
                    }
                } else if (pendingAction == "arrest")
                    currentPlayer->arrest(*validTargets[targetIndex]), currentPlayer->endTurn();
                else if (pendingAction == "sanction")
                    currentPlayer->sanction(*validTargets[targetIndex]), currentPlayer->endTurn();
                else if (pendingAction == "coup")
                    currentPlayer->coup(*validTargets[targetIndex]), currentPlayer->endTurn();

                if (game.isGameOver()) {
                    showTemporaryMessage("Winner: " + game.winner());
                    currentScreen = ScreenState::Welcome;
                    playerNames.clear();
                    createdPlayers.clear();
                    game.resetPlayers();
                    return;
                }
            } catch (const std::exception &e) {
                showTemporaryMessage(string("Action failed: ") + e.what());
            }
            pendingAction.clear();
            currentScreen = ScreenState::GameView;
            setupGameView();
        }
    }

    void drawGameUI(RenderWindow &window) {
        for (const auto &t: gameViewTexts) window.draw(t);

        for (const auto &[rect, label]: abilityButtons) {
            window.draw(rect);
            Text t(label, font, 16);
            t.setFillColor(Color::White);
            t.setPosition(rect.getPosition().x + 10, rect.getPosition().y + 8);
            window.draw(t);
        }

        for (const auto &[rect, label]: specialButtons) {
            window.draw(rect);
            Text t(label, font, 16);
            t.setFillColor(Color::White);
            t.setPosition(rect.getPosition().x + 10, rect.getPosition().y + 8);
            window.draw(t);
        }

        for (const auto &t: activePlayerList)
            window.draw(t);

        window.draw(terminateButton);
        window.draw(terminateText);
    }

    void handleActionClick(const string &actionName) {
        if (actionName == "peekCoins" || actionName == "arrest" || actionName == "sanction" || actionName == "coup") {
            setupTargetPopup(actionName);
            return;
        }

        try {
            if (actionName == "gather") currentPlayer->gather(), currentPlayer->endTurn();
            else if (actionName == "tax") currentPlayer->tax(), currentPlayer->endTurn();
            else if (actionName == "bribe") {
                currentPlayer->bribe();
                bribeExtraMove = true;
                return; // לא לסיים תור עדיין
            }

            if (game.isGameOver()) {
                showTemporaryMessage("Winner: " + game.winner());
                currentScreen = ScreenState::Welcome;
                playerNames.clear();
                createdPlayers.clear();
                game.resetPlayers();
                return;
            }

            setupGameView();
        } catch (const std::exception &e) {
            showTemporaryMessage(string("Action failed: ") + e.what());
        }
    }

    void runGUI() {
        RenderWindow window(VideoMode(800, 600), "Coup GUI");

        font.loadFromFile("GUI/fonts/arial.ttf");
        bgMenuTexture.loadFromFile("GUI/images/background_menu.png");
        menuBackground.setTexture(bgMenuTexture);
        bgGameTexture.loadFromFile("GUI/images/game_background.png");
        gameBackground.setTexture(bgGameTexture);

        // מסכי Welcome ו־ChooseCount
        RectangleShape playButton(Vector2f(200, 60));
        playButton.setPosition(300, 200);
        playButton.setFillColor(Color::Green);
        Text playText("PLAY NOW", font, 24);
        playText.setPosition(330, 215);
        playText.setFillColor(Color::Black);

        RectangleShape exitButton(Vector2f(200, 60));
        exitButton.setPosition(300, 300);
        exitButton.setFillColor(Color::Red);
        Text exitText("EXIT", font, 24);
        exitText.setPosition(370, 315);
        exitText.setFillColor(Color::Black);

        RectangleShape plusButton(Vector2f(60, 50));
        plusButton.setPosition(460, 200);
        plusButton.setFillColor(Color::Green);
        Text plusText("+", font, 30);
        plusText.setPosition(480, 205);
        plusText.setFillColor(Color::Black);

        RectangleShape minusButton(Vector2f(60, 50));
        minusButton.setPosition(280, 200);
        minusButton.setFillColor(Color::Red);
        Text minusText("-", font, 30);
        minusText.setPosition(300, 205);
        minusText.setFillColor(Color::Black);

        RectangleShape confirmCount(Vector2f(150, 50));
        confirmCount.setPosition(325, 300);
        confirmCount.setFillColor(Color::Blue);
        Text confirmText("OK", font, 24);
        confirmText.setPosition(380, 310);
        confirmText.setFillColor(Color::White);

        RectangleShape startButton(Vector2f(180, 50));
        startButton.setPosition(310, 500);
        startButton.setFillColor(Color::Magenta);
        Text startText("Start Game", font, 24);
        startText.setPosition(330, 510);
        startText.setFillColor(Color::White);

        RectangleShape backButton(Vector2f(140, 40));
        backButton.setPosition(30, 500);
        backButton.setFillColor(Color::Blue);
        Text backText("Back", font, 22);
        backText.setPosition(60, 510);
        backText.setFillColor(Color::White);

        RectangleShape startGameButton(Vector2f(180, 50));
        startGameButton.setPosition(300, 450);
        startGameButton.setFillColor(Color::Green);
        Text startGameText("START GAME", font, 24);
        startGameText.setPosition(320, 460);
        startGameText.setFillColor(Color::Black);

        RectangleShape mainMenuButton(Vector2f(220, 50));
        mainMenuButton.setPosition(280, 520);
        mainMenuButton.setFillColor(Color::Red);
        Text menuText("EXIT TO MAIN MENU", font, 22);
        menuText.setPosition(290, 530);
        menuText.setFillColor(Color::White);

        vector<RectangleShape> inputBoxes;
        Text chooseText("Choose number of participants:", font, 28);
        chooseText.setPosition(150, 100);
        chooseText.setFillColor(Color::White);
        Text countText(to_string(participantCount), font, 40);
        countText.setPosition(380, 200);
        countText.setFillColor(Color::Yellow);

        Text enterNamesTitle("Enter player names:", font, 32);
        enterNamesTitle.setPosition(240, 20);
        enterNamesTitle.setFillColor(Color::White);
        Text errorText("", font, 22);
        errorText.setPosition(200, 460);
        errorText.setFillColor(Color::Red);

        setupInputFields(inputBoxes);

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mouse(Mouse::getPosition(window));

                    if (currentScreen == ScreenState::Welcome) {
                        if (playButton.getGlobalBounds().contains(mouse)) currentScreen = ScreenState::ChooseCount;
                        else if (exitButton.getGlobalBounds().contains(mouse)) window.close();
                    } else if (currentScreen == ScreenState::ChooseCount) {
                        if (plusButton.getGlobalBounds().contains(mouse) && participantCount < 6) participantCount++;
                        if (minusButton.getGlobalBounds().contains(mouse) && participantCount > 2) participantCount--;
                        countText.setString(to_string(participantCount));
                        if (confirmCount.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::EnterNames;
                            setupInputFields(inputBoxes);
                        }
                    } else if (currentScreen == ScreenState::EnterNames) {
                        if (backButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::ChooseCount;
                            playerNames.clear();
                            nameTexts.clear();
                            nameLabels.clear();
                        } else {
                            for (size_t i = 0; i < inputBoxes.size(); ++i)
                                if (inputBoxes[i].getGlobalBounds().contains(mouse)) activeInputIndex = i;

                            if (startButton.getGlobalBounds().contains(mouse)) {
                                createdPlayers.clear();
                                errorMessage.clear();
                                bool success = true;
                                try {
                                    for (const auto &name: playerNames)
                                        createdPlayers.push_back(randomPlayer(game, name));
                                } catch (const std::exception &e) {
                                    errorMessage = e.what();
                                    createdPlayers.clear();
                                    game.resetPlayers();
                                    success = false;
                                }
                                if (success) {
                                    setupSummary();
                                    currentScreen = ScreenState::Summary;
                                }
                            }
                        }
                    } else if (currentScreen == ScreenState::Summary) {
                        if (startGameButton.getGlobalBounds().contains(mouse)) {
                            setupGameView();
                            currentScreen = ScreenState::GameView;
                        } else if (mainMenuButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::Welcome;
                            playerNames.clear();
                            createdPlayers.clear();
                            game.resetPlayers();
                        }
                    } else if (currentScreen == ScreenState::GameView) {
                        if (terminateButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::Welcome;
                            playerNames.clear();
                            createdPlayers.clear();
                            game.resetPlayers();
                        }

                        for (auto &[rect, action]: abilityButtons)
                            if (rect.getGlobalBounds().contains(mouse)) handleActionClick(action);

                        for (auto &[rect, action]: specialButtons)
                            if (rect.getGlobalBounds().contains(mouse)) handleActionClick(action);
                    } else if (currentScreen == ScreenState::TargetPopup) {
                        handleTargetPopupClick(mouse);
                    }
                }

                if (currentScreen == ScreenState::EnterNames && event.type == Event::TextEntered) {
                    if (event.text.unicode == '\b') {
                        if (!playerNames[activeInputIndex].empty())
                            playerNames[activeInputIndex].pop_back();
                    } else if (event.text.unicode < 128 && event.text.unicode != '\r') {
                        playerNames[activeInputIndex] += static_cast<char>(event.text.unicode);
                    }
                    nameTexts[activeInputIndex].setString(playerNames[activeInputIndex]);
                }
            }

            window.clear();

            if (currentScreen == ScreenState::Welcome) {
                window.draw(menuBackground);
                window.draw(playButton);
                window.draw(playText);
                window.draw(exitButton);
                window.draw(exitText);
            } else if (currentScreen == ScreenState::ChooseCount) {
                window.draw(gameBackground);
                window.draw(chooseText);
                window.draw(countText);
                window.draw(plusButton);
                window.draw(plusText);
                window.draw(minusButton);
                window.draw(minusText);
                window.draw(confirmCount);
                window.draw(confirmText);
            } else if (currentScreen == ScreenState::EnterNames) {
                window.draw(gameBackground);
                window.draw(enterNamesTitle);
                for (size_t i = 0; i < participantCount; ++i) {
                    window.draw(nameLabels[i]);
                    window.draw(inputBoxes[i]);
                    window.draw(nameTexts[i]);
                }
                window.draw(startButton);
                window.draw(startText);
                window.draw(backButton);
                window.draw(backText);
                if (!errorMessage.empty()) {
                    errorText.setString(errorMessage);
                    window.draw(errorText);
                }
            } else if (currentScreen == ScreenState::Summary) {
                window.draw(gameBackground);
                for (const auto &t: summaryTexts) window.draw(t);
                window.draw(startGameButton);
                window.draw(startGameText);
                window.draw(mainMenuButton);
                window.draw(menuText);
            } else if (currentScreen == ScreenState::GameView) {
                window.draw(gameBackground);
                drawGameUI(window);
                drawTemporaryMessage(window);
            } else if (currentScreen == ScreenState::TargetPopup) {
                drawTargetPopup(window);
            }

            window.display();
        }
    }
}
