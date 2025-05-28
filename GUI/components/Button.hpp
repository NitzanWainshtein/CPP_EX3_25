// Email: nitzanwa@gmail.com

#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <cmath> // Added to ensure sin() and other math functions are available

namespace coup {
    namespace gui {

        class Button {
        public:
            Button(float x, float y, float width, float height,
                   const sf::Font& font, const std::string& text,
                   std::function<void()> onClick);

            void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
            void update(const sf::RenderWindow& window);
            void draw(sf::RenderWindow& window);

            void setEnabled(bool enable);
            void setText(const std::string& text);
            void setPosition(float x, float y);
            void setColors(const sf::Color& normal, const sf::Color& hover,
                           const sf::Color& pressed, const sf::Color& disabled);

            bool isEnabled() const { return enabled; }

        private:
            sf::RectangleShape background;
            sf::Text label;
            std::function<void()> callback;

            sf::Color normalColor;
            sf::Color hoverColor;
            sf::Color pressedColor;
            sf::Color disabledColor;

            bool enabled = true;
            bool hovered = false;
            bool pressed = false;
        };

    } // namespace gui
} // namespace coup