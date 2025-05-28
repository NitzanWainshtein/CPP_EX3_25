// GUI/components/Popup.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include "Button.hpp"

namespace coup {
namespace gui {

class Popup {
private:
    sf::RectangleShape overlay;
    sf::RectangleShape window;
    sf::Text messageText;
    Button okButton;
    bool visible = false;
    std::function<void()> onClose;

public:
    Popup(const sf::Font& font)
        : okButton(590, 420, 100, 40, font, "OK", [this]() { hide(); }) {

        // Dark overlay
        overlay.setSize({1280, 900});
        overlay.setFillColor(sf::Color(0, 0, 0, 180));

        // Popup window
        window.setSize({500, 200});
        window.setPosition(390, 300);
        window.setFillColor(sf::Color(40, 40, 50));
        window.setOutlineThickness(3);
        window.setOutlineColor(sf::Color::Red);

        // Message text
        messageText.setFont(font);
        messageText.setCharacterSize(20);
        messageText.setFillColor(sf::Color::White);
        messageText.setPosition(410, 330);

        // Style OK button
        okButton.setColors(
            sf::Color(80, 80, 80),
            sf::Color(100, 100, 100),
            sf::Color(60, 60, 60),
            sf::Color(50, 50, 50)
        );
    }

    void show(const std::string& message, std::function<void()> callback = nullptr) {
        messageText.setString(message);
        visible = true;
        onClose = callback;

        // Center message if short
        if (message.length() < 50) {
            sf::FloatRect bounds = messageText.getLocalBounds();
            messageText.setPosition(640 - bounds.width/2, 350);
        }
    }

    void hide() {
        visible = false;
        if (onClose) {
            onClose();
            onClose = nullptr;
        }
    }

    bool isVisible() const { return visible; }

    void handleEvent(const sf::Event& event, const sf::RenderWindow& win) {
        if (!visible) return;
        okButton.handleEvent(event, win);
    }

    void update(const sf::RenderWindow& win) {
        if (!visible) return;
        okButton.update(win);
    }

    void draw(sf::RenderWindow& win) {
        if (!visible) return;

        win.draw(overlay);
        win.draw(window);
        win.draw(messageText);
        okButton.draw(win);
    }
};

} // namespace gui
} // namespace coup