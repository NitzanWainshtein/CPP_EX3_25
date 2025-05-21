// Email: nitzanwa@gmail.com

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <memory>
#include "../GameLogic/Game.hpp"
#include "../GameLogic/PlayerFactory.hpp"


using namespace coup;

enum class GameState {
    StartScreen,
    PlayerCount,
    PlayerNames,
    GameRunning,
    Exit
};

class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;

    Button(float x, float y, float width, float height, const sf::Font& font, const std::string& label) {
        shape.setPosition(x, y);
        shape.setSize({width, height});
        shape.setFillColor(sf::Color(70, 70, 70));

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(x + (width - textBounds.width) / 2 - textBounds.left, y + (height - textBounds.height) / 2 - textBounds.top);
    }

    bool isClicked(sf::Vector2i mousePos) {
        return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};

int runGUI() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Coup Game");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    GameState state = GameState::StartScreen;
    int playerCount = 0;
    std::vector<std::string> playerNames;
    std::vector<sf::Text> nameInputs(6);
    std::vector<std::string> nameBuffers(6);
    int nameIndex = 0;

    Button startBtn(400, 250, 200, 60, font, "Start Game");
    Button exitBtn(400, 350, 200, 60, font, "Exit");

    std::vector<Button> countButtons;
    for (int i = 2; i <= 6; ++i) {
        countButtons.emplace_back(180 + (i-2)*120, 300, 100, 60, font, std::to_string(i));
    }

    for (int i = 0; i < 6; ++i) {
        nameInputs[i].setFont(font);
        nameInputs[i].setCharacterSize(22);
        nameInputs[i].setFillColor(sf::Color::White);
        nameInputs[i].setPosition(300, 120 + i * 60);
    }

    std::unique_ptr<Game> game;
    std::vector<std::shared_ptr<Player>> players;

    std::vector<Button> actionButtons;
    actionButtons.emplace_back(50, 600, 150, 40, font, "Income");
    actionButtons.emplace_back(220, 600, 150, 40, font, "Foreign Aid");
    actionButtons.emplace_back(390, 600, 150, 40, font, "Coup");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == GameState::StartScreen) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (startBtn.isClicked(sf::Mouse::getPosition(window))) {
                        state = GameState::PlayerCount;
                    } else if (exitBtn.isClicked(sf::Mouse::getPosition(window))) {
                        window.close();
                    }
                }
            }
            else if (state == GameState::PlayerCount) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    for (int i = 0; i < countButtons.size(); ++i) {
                        if (countButtons[i].isClicked(sf::Mouse::getPosition(window))) {
                            playerCount = i + 2;
                            state = GameState::PlayerNames;
                        }
                    }
                }
            }
            else if (state == GameState::PlayerNames) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\b') {
                        if (!nameBuffers[nameIndex].empty())
                            nameBuffers[nameIndex].pop_back();
                    } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                        if (++nameIndex == playerCount) {
                            playerNames.assign(nameBuffers.begin(), nameBuffers.begin() + playerCount);
                            game = std::make_unique<Game>();
                            players = PlayerFactory::createRandomPlayers(playerNames, *game);
                            for (const auto& p : players) {
                                game->addPlayer(p);
                            }
                            game->start();
                            state = GameState::GameRunning;
                        }
                    } else if (event.text.unicode < 128) {
                        nameBuffers[nameIndex] += static_cast<char>(event.text.unicode);
                    }
                    nameInputs[nameIndex].setString("Player " + std::to_string(nameIndex+1) + ": " + nameBuffers[nameIndex]);
                }
            }
            else if (state == GameState::GameRunning) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    for (int i = 0; i < actionButtons.size(); ++i) {
                        if (actionButtons[i].isClicked(sf::Mouse::getPosition(window))) {
                            std::shared_ptr<Player> current = game->currentPlayer();
                            try {
                                if (i == 0) current->income();
                                else if (i == 1) current->foreign_aid();
                                else if (i == 2) {
                                    // Simple target logic: coup the next player
                                    auto target = players[(game->turn() + 1) % players.size()];
                                    current->coup(target);
                                }
                            } catch (const std::exception& e) {
                                std::cerr << "Action error: " << e.what() << std::endl;
                            }
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(20, 20, 30));

        if (state == GameState::StartScreen) {
            startBtn.draw(window);
            exitBtn.draw(window);
        }
        else if (state == GameState::PlayerCount) {
            for (auto& b : countButtons)
                b.draw(window);
        }
        else if (state == GameState::PlayerNames) {
            for (int i = 0; i < playerCount; ++i)
                window.draw(nameInputs[i]);
        }
        else if (state == GameState::GameRunning) {
            sf::Text header("Game Started! Players:", font, 28);
            header.setPosition(100, 40);
            window.draw(header);

            for (int i = 0; i < players.size(); ++i) {
                sf::Text playerText(font, players[i]->name() + " (" + players[i]->role()->roleName() + ")", 24);
                playerText.setPosition(120, 100 + i * 40);
                if (i == game->turn())
                    playerText.setFillColor(sf::Color::Green);
                window.draw(playerText);
            }

            for (auto& b : actionButtons)
                b.draw(window);
        }

        window.display();
    }

    return 0;
}
