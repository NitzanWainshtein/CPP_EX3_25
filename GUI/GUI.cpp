#include "GUI.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "../GameLogic/Game.hpp"
#include "../GameLogic/PlayerFactory.hpp"
#include "../Players/Player.hpp"

using namespace sf;
using namespace std;

namespace coup {

    enum class ScreenState {
        Welcome,
        ChooseCount,
        EnterNames,
        Summary,
        GameView
    };

    static ScreenState currentScreen = ScreenState::Welcome;
    static int participantCount = 2;
    static vector<string> playerNames;
    static vector<Text> nameTexts;
    static vector<Text> nameLabels;
    static vector<Text> summaryTexts;
    static vector<Text> activePlayersTexts;
    static int activeInputIndex = 0;
    static Game game;
    static vector<Player*> createdPlayers;
    static string errorMessage = "";

    void setupInputFields(Font &font, vector<RectangleShape> &inputBoxes) {
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

    void setupSummary(Font &font) {
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

    void setupGameView(Font &font) {
        activePlayersTexts.clear();

        Text turnTitle("Turn:", font, 30);
        turnTitle.setFillColor(Color::White);
        turnTitle.setPosition(100, 50);
        activePlayersTexts.push_back(turnTitle);

        Text currentTurn(game.turn(), font, 30);
        currentTurn.setFillColor(Color::Cyan);
        currentTurn.setPosition(200, 50);
        activePlayersTexts.push_back(currentTurn);

        Text playersTitle("Active Players:", font, 26);
        playersTitle.setFillColor(Color::White);
        playersTitle.setPosition(100, 120);
        activePlayersTexts.push_back(playersTitle);

        vector<string> actives = game.players();
        for (size_t i = 0; i < actives.size(); ++i) {
            Text t(actives[i], font, 24);
            t.setFillColor(Color::Green);
            t.setPosition(120, 160 + static_cast<int>(i) * 30);
            activePlayersTexts.push_back(t);
        }
    }

    void runGUI() {
        RenderWindow window(VideoMode(800, 600), "Coup GUI");

        Font font;
        if (!font.loadFromFile("GUI/fonts/arial.ttf")) {
            cerr << "Failed to load font" << endl;
            return;
        }

        Texture bgMenuTexture;
        bgMenuTexture.loadFromFile("GUI/images/background_menu.png");
        Sprite menuBackground(bgMenuTexture);

        Texture bgGameTexture;
        bgGameTexture.loadFromFile("GUI/images/game_background.png");
        Sprite gameBackground(bgGameTexture);

        Text title("Welcome to Coup!", font, 48);
        title.setPosition(150, 50);
        title.setFillColor(Color::White);

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

        RectangleShape increaseButton(Vector2f(60, 50));
        increaseButton.setPosition(460, 200);
        increaseButton.setFillColor(Color::Green);

        Text plusText("+", font, 30);
        plusText.setPosition(480, 205);
        plusText.setFillColor(Color::Black);

        RectangleShape decreaseButton(Vector2f(60, 50));
        decreaseButton.setPosition(280, 200);
        decreaseButton.setFillColor(Color::Red);

        Text minusText("-", font, 30);
        minusText.setPosition(300, 205);
        minusText.setFillColor(Color::Black);

        RectangleShape confirmButton(Vector2f(150, 50));
        confirmButton.setPosition(325, 300);
        confirmButton.setFillColor(Color::Blue);

        Text confirmText("OK", font, 24);
        confirmText.setPosition(380, 310);
        confirmText.setFillColor(Color::White);

        Text enterNamesTitle("Enter player names:", font, 32);
        enterNamesTitle.setPosition(240, 20);
        enterNamesTitle.setFillColor(Color::White);

        RectangleShape startButton(Vector2f(180, 50));
        startButton.setPosition(310, 500);
        startButton.setFillColor(Color::Magenta);

        Text startText("Start Game", font, 24);
        startText.setPosition(330, 510);
        startText.setFillColor(Color::White);

        Text errorText("", font, 22);
        errorText.setPosition(200, 460);
        errorText.setFillColor(Color::Red);

        setupInputFields(font, inputBoxes);

        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                    Vector2f mouse(Mouse::getPosition(window));

                    if (currentScreen == ScreenState::Welcome) {
                        if (playButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::ChooseCount;
                        } else if (exitButton.getGlobalBounds().contains(mouse)) {
                            window.close();
                        }
                    } else if (currentScreen == ScreenState::ChooseCount) {
                        if (increaseButton.getGlobalBounds().contains(mouse) && participantCount < 6)
                            participantCount++, countText.setString(to_string(participantCount));
                        if (decreaseButton.getGlobalBounds().contains(mouse) && participantCount > 2)
                            participantCount--, countText.setString(to_string(participantCount));
                        if (confirmButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::EnterNames;
                            setupInputFields(font, inputBoxes);
                        }
                    } else if (currentScreen == ScreenState::EnterNames) {
                        if (backButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::ChooseCount;
                            playerNames.clear();
                            nameTexts.clear();
                            nameLabels.clear();
                        } else {
                            for (size_t i = 0; i < inputBoxes.size(); ++i) {
                                if (inputBoxes[i].getGlobalBounds().contains(mouse)) {
                                    activeInputIndex = i;
                                }
                            }
                            if (startButton.getGlobalBounds().contains(mouse)) {
                                createdPlayers.clear();
                                errorMessage.clear();
                                bool success = true;
                                try {
                                    for (const auto& name : playerNames) {
                                        createdPlayers.push_back(randomPlayer(game, name));
                                    }
                                } catch (const std::exception &e) {
                                    errorMessage = e.what();
                                    createdPlayers.clear();
                                    game.resetPlayers();
                                    success = false;
                                }
                                if (success) {
                                    setupSummary(font);
                                    currentScreen = ScreenState::Summary;
                                }
                            }
                        }
                    } else if (currentScreen == ScreenState::Summary) {
                        if (startGameButton.getGlobalBounds().contains(mouse)) {
                            setupGameView(font);
                            currentScreen = ScreenState::GameView;
                        } else if (mainMenuButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::Welcome;
                            createdPlayers.clear();
                            playerNames.clear();
                            nameTexts.clear();
                            nameLabels.clear();
                            summaryTexts.clear();
                            game.resetPlayers();
                        }
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
                window.draw(title);
                window.draw(playButton); window.draw(playText);
                window.draw(exitButton); window.draw(exitText);
            } else {
                window.draw(gameBackground);

                if (currentScreen == ScreenState::ChooseCount) {
                    window.draw(chooseText);
                    window.draw(countText);
                    window.draw(increaseButton); window.draw(plusText);
                    window.draw(decreaseButton); window.draw(minusText);
                    window.draw(confirmButton); window.draw(confirmText);
                } else if (currentScreen == ScreenState::EnterNames) {
                    window.draw(enterNamesTitle);
                    for (size_t i = 0; i < participantCount; ++i) {
                        window.draw(nameLabels[i]);
                        window.draw(inputBoxes[i]);
                        window.draw(nameTexts[i]);
                    }
                    window.draw(startButton);
                    window.draw(startText);
                    window.draw(backButton); window.draw(backText);
                    if (!errorMessage.empty()) {
                        errorText.setString(errorMessage);
                        window.draw(errorText);
                    }
                } else if (currentScreen == ScreenState::Summary) {
                    for (const auto &text : summaryTexts) window.draw(text);
                    window.draw(startGameButton); window.draw(startGameText);
                    window.draw(mainMenuButton); window.draw(menuText);
                } else if (currentScreen == ScreenState::GameView) {
                    for (const auto &text : activePlayersTexts) window.draw(text);
                }
            }

            window.display();
        }
    }

} // namespace coup
