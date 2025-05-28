// GUI/components/BlockingDialog.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "Button.hpp"

namespace coup {
namespace gui {

class BlockingDialog {
private:
    sf::RectangleShape overlay;
    sf::RectangleShape window;
    sf::Text titleText;
    sf::Text messageText;
    Button blockButton;
    Button passButton;

    bool visible = false;
    std::function<void(bool)> callback;

public:
    BlockingDialog(const sf::Font& font)
        : blockButton(440, 400, 120, 50, font, "Block", [this]() { respond(true); }),
          passButton(580, 400, 120, 50, font, "Pass", [this]() { respond(false); }) {

        // Dark overlay
        overlay.setSize({1280, 900});
        overlay.setFillColor(sf::Color(0, 0, 0, 180));

        // Dialog window
        window.setSize({600, 300});
        window.setPosition(340, 250);
        window.setFillColor(sf::Color(40, 40, 50));
        window.setOutlineThickness(3);
        window.setOutlineColor(sf::Color::Yellow);

        // Title
        titleText.setFont(font);
        titleText.setCharacterSize(24);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setPosition(360, 270);

        // Message
        messageText.setFont(font);
        messageText.setCharacterSize(18);
        messageText.setFillColor(sf::Color::White);
        messageText.setPosition(360, 320);

        // Style buttons
        blockButton.setColors(
            sf::Color(120, 60, 60),
            sf::Color(150, 80, 80),
            sf::Color(90, 40, 40),
            sf::Color(50, 50, 50)
        );

        passButton.setColors(
            sf::Color(60, 120, 60),
            sf::Color(80, 150, 80),
            sf::Color(40, 90, 40),
            sf::Color(50, 50, 50)
        );
    }

    void show(const std::string& blocker, const std::string& action,
              const std::string& actor, std::function<void(bool)> cb) {
        titleText.setString("BLOCKING OPPORTUNITY");
        messageText.setString(blocker + " can block " + actor + "'s " + action + "!");
        visible = true;
        callback = cb;
    }

    void hide() {
        visible = false;
        callback = nullptr;
    }

    bool isVisible() const { return visible; }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& win) {
        if (!visible) return;
        blockButton.handleEvent(event, win);
        passButton.handleEvent(event, win);
    }

    void update(const sf::RenderWindow& win) {
        if (!visible) return;
        blockButton.update(win);
        passButton.update(win);
    }

    void draw(sf::RenderWindow& win) {
        if (!visible) return;

        win.draw(overlay);
        win.draw(window);
        win.draw(titleText);
        win.draw(messageText);
        blockButton.draw(win);
        passButton.draw(win);
    }

private:
    void respond(bool block) {
        hide();
        if (callback) {
            callback(block);
        }
    }
};

} // namespace gui
} // namespace coup