// Email: nitzanwa@gmail.com

#include "GUI.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <optional>
#include <chrono>
#include <algorithm>

#include "../GameLogic/Game.hpp"
#include "../GameLogic/PlayerFactory.hpp"
#include "../Players/Player.hpp"
#include "../Players/Roles/Spy.hpp"
#include "../Players/Roles/Merchant.hpp"
#include "../Players/Roles/General.hpp"
#include "../Players/Roles/Judge.hpp"
#include "../Players/Roles/Governor.hpp"
#include "../Players/Roles/Baron.hpp"

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
        BlockDecisionPopup
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

    static Player *currentPlayer = nullptr;
    static string pendingAction = "";
    static bool bribeExtraMove = false;
    static vector<pair<RectangleShape, string>> abilityButtons;
    static vector<pair<RectangleShape, string>> specialButtons;
    static vector<Player *> validTargets;
    static size_t targetIndex = 0;
    static RectangleShape popupBox, confirmButton, cancelButton;
    static Text popupTitle, targetNameText, confirmText, cancelText, leftArrowText, rightArrowText;
    static RectangleShape leftArrow, rightArrow;

    static Font font;
    static Texture bgMenuTexture, bgGameTexture;
    static Sprite menuBackground, gameBackground;
    static RectangleShape terminateButton;
    static Text terminateText;

    // Blocking popup state
    static bool blockPopupVisible = false;
    static Player* blockingPlayer = nullptr;
    static Player* actorPlayer = nullptr;
    static Player* actionTarget = nullptr;
    static ActionType actionToBlock = ActionType::None;

    static RectangleShape blockPopupBox, allowButton, denyButton;
    static Text blockPromptText, allowText, denyText;

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

    void prepareBlockPopup(Player* responder, Player* actor, ActionType action, Player* target) {
        blockingPlayer = responder;
        actorPlayer = actor;
        actionToBlock = action;
        actionTarget = target;
        blockPopupVisible = true;

        blockPopupBox = RectangleShape(Vector2f(520, 160));
        blockPopupBox.setFillColor(Color(50, 50, 50, 230));
        blockPopupBox.setOutlineColor(Color::White);
        blockPopupBox.setOutlineThickness(2);
        blockPopupBox.setPosition(140, 200);

        string act = (action == ActionType::Bribe ? "bribe" : "coup");
        string prompt = responder->getName() + ": Block " + act + " by " + actor->getName() + "?";

        blockPromptText = Text(prompt, font, 20);
        blockPromptText.setFillColor(Color::White);
        blockPromptText.setPosition(160, 220);

        allowButton = RectangleShape(Vector2f(120, 40));
        allowButton.setFillColor(Color::Green);
        allowButton.setPosition(170, 280);
        allowText = Text("Block", font, 20);
        allowText.setFillColor(Color::Black);
        allowText.setPosition(200, 287);

        denyButton = RectangleShape(Vector2f(120, 40));
        denyButton.setFillColor(Color::Red);
        denyButton.setPosition(380, 280);
        denyText = Text("Allow", font, 20);
        denyText.setFillColor(Color::White);
        denyText.setPosition(410, 287);
    }
    void drawBlockPopup(RenderWindow &window) {
        if (!blockPopupVisible) return;
        window.draw(blockPopupBox);
        window.draw(blockPromptText);
        window.draw(allowButton);
        window.draw(allowText);
        window.draw(denyButton);
        window.draw(denyText);
    }
    // Forward declarations
    void setupGameView();
    void setupTargetPopup(const std::string &actionName);

    void handleBlockPopupClick(Vector2f mouse) {
        if (!blockPopupVisible) return;

        if (allowButton.getGlobalBounds().contains(mouse)) {
            if (actionToBlock == ActionType::Bribe) {
                auto* judge = dynamic_cast<Judge*>(blockingPlayer);
                if (judge && judge->tryBlockBribe(*actorPlayer)) {
                    actorPlayer->blockLastAction();
                    showTemporaryMessage("Bribe was blocked by Judge!");
                }
            } else if (actionToBlock == ActionType::Coup) {
                auto* general = dynamic_cast<General*>(blockingPlayer);
                if (general && general->tryBlockCoup(*actorPlayer, *actionTarget)) {
                    actorPlayer->blockLastAction();
                    showTemporaryMessage("Coup was blocked by General!");
                }
            }
        }

        blockPopupVisible = false;
    }

    void Game::requestImmediateResponse(Player* actor, ActionType action, Player* target) {
        for (Player* responder : player_list) {
            if (!responder || responder == actor || !isAlive(*responder)) continue;

            if (action == ActionType::Bribe && responder->getRoleName() == "Judge") {
                prepareBlockPopup(responder, actor, action, target);
                return;
            }
            if (action == ActionType::Coup && responder->getRoleName() == "General") {
                prepareBlockPopup(responder, actor, action, target);
                return;
            }
        }
    }

    void handleBlockPopupMouse(Vector2f mouse) {
        handleBlockPopupClick(mouse);
        setupGameView();  // Refresh after response
    }

    void checkGameOver() {
        if (game.isGameOver()) {
            showTemporaryMessage("Winner: " + game.winner());
            currentScreen = ScreenState::Welcome;
            playerNames.clear();
            createdPlayers.clear();
            game.resetPlayers();
        }
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
        string role = currentPlayer->getRoleName();
        if (role == "Spy") specials.push_back("peekCoins");
        else if (role == "Governor") specials.push_back("undoTax");
        else if (role == "General") specials.push_back("blockCoup");
        else if (role == "Judge") specials.push_back("blockBribe");
        else if (role == "Baron") specials.push_back("invest");
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

    void handleSpecialActionClick(const string &special) {
        try {
            if (special == "peekCoins") {
                setupTargetPopup("peekCoins");
            } else if (special == "undoTax") {
                for (Player* p : createdPlayers) {
                    if (p && p != currentPlayer && p->getLastAction() == ActionType::Tax) {
                        currentPlayer->undo(*p);
                        showTemporaryMessage("Tax by " + p->getName() + " was canceled.");
                        return;
                    }
                }
                showTemporaryMessage("No tax to cancel.");
            } else if (special == "blockBribe") {
                for (Player* p : createdPlayers) {
                    if (p && p->getLastAction() == ActionType::Bribe) {
                        auto *judge = dynamic_cast<Judge *>(currentPlayer);
                        if (judge && judge->tryBlockBribe(*p)) {
                            showTemporaryMessage("Bribe by " + p->getName() + " was blocked.");
                        }
                        return;
                    }
                }
                showTemporaryMessage("No bribe to block.");
            } else if (special == "blockCoup") {
                for (Player* p : createdPlayers) {
                    if (p && p->getLastAction() == ActionType::Coup) {
                        auto *general = dynamic_cast<General *>(currentPlayer);
                        if (general && general->tryBlockCoup(*p, *currentPlayer)) {
                            showTemporaryMessage("Coup by " + p->getName() + " was blocked.");
                        }
                        return;
                    }
                }
                showTemporaryMessage("No coup to block.");
            } else if (special == "invest") {
                auto *baron = dynamic_cast<Baron *>(currentPlayer);
                if (baron) {
                    baron->invest();
                    showTemporaryMessage("Baron invested 3 coins.");
                }
            }

            checkGameOver();
            setupGameView();
        } catch (const std::exception &e) {
            showTemporaryMessage(string("Special action failed: ") + e.what());
        }
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
                return;
            }

            checkGameOver();
            setupGameView();
        } catch (const std::exception &e) {
            showTemporaryMessage(string("Action failed: ") + e.what());
        }
    }

    void handleGameViewClick(Vector2f mouse) {
        if (terminateButton.getGlobalBounds().contains(mouse)) {
            currentScreen = ScreenState::Welcome;
            playerNames.clear();
            createdPlayers.clear();
            game.resetPlayers();
        }

        for (auto &[rect, action]: abilityButtons)
            if (rect.getGlobalBounds().contains(mouse)) handleActionClick(action);

        for (auto &[rect, action]: specialButtons)
            if (rect.getGlobalBounds().contains(mouse)) handleSpecialActionClick(action);
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

                checkGameOver();
            } catch (const std::exception &e) {
                showTemporaryMessage(string("Action failed: ") + e.what());
            }
            pendingAction.clear();
            currentScreen = ScreenState::GameView;
            setupGameView();
        }
    }
    void runGUI() {
        RenderWindow window(VideoMode(800, 600), "Coup GUI");

        font.loadFromFile("GUI/fonts/arial.ttf");
        bgMenuTexture.loadFromFile("GUI/images/background_menu.png");
        menuBackground.setTexture(bgMenuTexture);
        bgGameTexture.loadFromFile("GUI/images/game_background.png");
        gameBackground.setTexture(bgGameTexture);

        vector<RectangleShape> inputBoxes;
        setupInputFields(inputBoxes);

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mouse(Mouse::getPosition(window));

                    if (blockPopupVisible) {
                        handleBlockPopupMouse(mouse);
                        continue;
                    }

                    if (currentScreen == ScreenState::Welcome) {
                        // Optional buttons...
                    } else if (currentScreen == ScreenState::GameView) {
                        handleGameViewClick(mouse);
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
                // Draw buttons...
            } else if (currentScreen == ScreenState::GameView) {
                window.draw(gameBackground);
                drawGameUI(window);
                drawTemporaryMessage(window);
                drawBlockPopup(window);
            } else if (currentScreen == ScreenState::TargetPopup) {
                drawTargetPopup(window);
            }

            window.display();
        }
    }
}
