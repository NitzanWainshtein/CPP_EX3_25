// GUI/components/Button.cpp
#include "Button.hpp"

namespace coup {
namespace gui {

Button::Button(float x, float y, float width, float height,
               const sf::Font& font, const std::string& text,
               std::function<void()> onClick)
    : callback(onClick),
      normalColor(90, 90, 90),
      hoverColor(120, 120, 120),
      pressedColor(60, 60, 60),
      disabledColor(50, 50, 50, 128) {

    // Setup background
    background.setPosition(x, y);
    background.setSize({width, height});
    background.setFillColor(normalColor);
    background.setOutlineThickness(2);
    background.setOutlineColor(sf::Color(150, 150, 150));

    // Setup text
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(18);
    label.setFillColor(sf::Color::White);

    // Center text
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setPosition(
        x + (width - textBounds.width) / 2 - textBounds.left,
        y + (height - textBounds.height) / 2 - textBounds.top
    );
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (!enabled) return;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    bool mouseOver = background.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));

    if (event.type == sf::Event::MouseButtonPressed && mouseOver) {
        pressed = true;
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (pressed && mouseOver && callback) {
            callback();
        }
        pressed = false;
    }
}

void Button::update(const sf::RenderWindow& window) {
    if (!enabled) {
        background.setFillColor(disabledColor);
        return;
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    hovered = background.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));

    if (pressed) {
        background.setFillColor(pressedColor);
    } else if (hovered) {
        background.setFillColor(hoverColor);
    } else {
        background.setFillColor(normalColor);
    }
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(label);
}

void Button::setEnabled(bool enable) {
    enabled = enable;
    label.setFillColor(enabled ? sf::Color::White : sf::Color(150, 150, 150));
}

void Button::setText(const std::string& text) {
    label.setString(text);
    // Re-center text
    sf::FloatRect textBounds = label.getLocalBounds();
    sf::Vector2f bgPos = background.getPosition();
    sf::Vector2f bgSize = background.getSize();
    label.setPosition(
        bgPos.x + (bgSize.x - textBounds.width) / 2 - textBounds.left,
        bgPos.y + (bgSize.y - textBounds.height) / 2 - textBounds.top
    );
}

void Button::setPosition(float x, float y) {
    background.setPosition(x, y);
    // Update text position
    setText(label.getString());
}

void Button::setColors(const sf::Color& normal, const sf::Color& hover,
                       const sf::Color& pressed, const sf::Color& disabled) {
    normalColor = normal;
    hoverColor = hover;
    pressedColor = pressed;
    disabledColor = disabled;
}

} // namespace gui
} // namespace coup