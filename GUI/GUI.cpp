#include "GUI.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

namespace coup {

    enum class ScreenState {
        Welcome,
        ChooseCount,
        EnterNames,
        Summary
    };

    static ScreenState currentScreen = ScreenState::Welcome;
    static int participantCount = 2;
    static vector<string> playerNames;
    static vector<Text> nameTexts;
    static int activeInputIndex = 0;

    void runGUI() {
        RenderWindow window(VideoMode(800, 600), "Coup GUI");

        // Load font
        Font font;
        if (!font.loadFromFile("GUI/fonts/arial.ttf")) {
            cerr << "Failed to load font" << endl;
            return;
        }

        // Load backgrounds
        Texture bgMenuTexture;
        if (!bgMenuTexture.loadFromFile("GUI/images/background_menu.png")) {
            cerr << "Failed to load menu background" << endl;
        }
        Sprite menuBackground(bgMenuTexture);

        Texture bgGameTexture;
        if (!bgGameTexture.loadFromFile("GUI/images/game_background.png")) {
            cerr << "Failed to load game background" << endl;
        }
        Sprite gameBackground(bgGameTexture);

        // Welcome Screen UI
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

        // Choose Count Screen
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

        // Input Setup
        Text enterNamesTitle("Enter player names:", font, 32);
        enterNamesTitle.setPosition(240, 50);
        enterNamesTitle.setFillColor(Color::White);

        vector<RectangleShape> inputBoxes;

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
                    }

                    if (currentScreen == ScreenState::ChooseCount) {
                        if (increaseButton.getGlobalBounds().contains(mouse) && participantCount < 6) {
                            participantCount++;
                            countText.setString(to_string(participantCount));
                        }
                        if (decreaseButton.getGlobalBounds().contains(mouse) && participantCount > 2) {
                            participantCount--;
                            countText.setString(to_string(participantCount));
                        }
                        if (confirmButton.getGlobalBounds().contains(mouse)) {
                            currentScreen = ScreenState::EnterNames;
                            playerNames = vector<string>(participantCount, "");
                            nameTexts.clear();
                            inputBoxes.clear();
                            activeInputIndex = 0;

                            for (int i = 0; i < participantCount; ++i) {
                                RectangleShape box(Vector2f(400, 40));
                                box.setPosition(200, 120 + i * 60);
                                box.setFillColor(Color(255, 255, 255, 200));
                                inputBoxes.push_back(box);

                                Text text("", font, 24);
                                text.setFillColor(Color::Black);
                                text.setPosition(210, 125 + i * 60);
                                nameTexts.push_back(text);
                            }
                        }
                    }

                    if (currentScreen == ScreenState::EnterNames) {
                        for (int i = 0; i < inputBoxes.size(); ++i) {
                            if (inputBoxes[i].getGlobalBounds().contains(mouse)) {
                                activeInputIndex = i;
                            }
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

            // Draw
            window.clear();
            if (currentScreen == ScreenState::Welcome)
                window.draw(menuBackground);
            else
                window.draw(gameBackground);

            if (currentScreen == ScreenState::Welcome) {
                window.draw(title);
                window.draw(playButton); window.draw(playText);
                window.draw(exitButton); window.draw(exitText);
            }

            else if (currentScreen == ScreenState::ChooseCount) {
                window.draw(chooseText);
                window.draw(countText);
                window.draw(increaseButton); window.draw(plusText);
                window.draw(decreaseButton); window.draw(minusText);
                window.draw(confirmButton); window.draw(confirmText);
            }

            else if (currentScreen == ScreenState::EnterNames) {
                window.draw(enterNamesTitle);
                for (int i = 0; i < participantCount; ++i) {
                    window.draw(inputBoxes[i]);
                    window.draw(nameTexts[i]);
                }
            }

            window.display();
        }
    }

} // namespace coup
