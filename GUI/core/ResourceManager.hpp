// GUI/core/ResourceManager.hpp
#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace coup {
namespace gui {

class ResourceManager {
private:
    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::Font> fonts;

    ResourceManager() = default;

public:
    // Singleton pattern
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Delete copy/move constructors
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Load resources
    bool loadTexture(const std::string& name, const std::string& filepath) {
        sf::Texture texture;
        if (texture.loadFromFile(filepath)) {
            textures[name] = std::move(texture);
            return true;
        }
        return false;
    }

    bool loadFont(const std::string& name, const std::string& filepath) {
        sf::Font font;
        if (font.loadFromFile(filepath)) {
            fonts[name] = std::move(font);
            return true;
        }
        return false;
    }

    // Get resources
    sf::Texture& getTexture(const std::string& name) {
        return textures.at(name);
    }

    sf::Font& getFont(const std::string& name) {
        return fonts.at(name);
    }

    // Initialize all resources
    bool loadAllResources() {
        // Load backgrounds
        if (!loadTexture("menu_bg", "GUI/images/menu_bg.png")) {
            std::cerr << "Warning: Could not load menu_bg.png" << std::endl;
        }
        if (!loadTexture("game_bg", "GUI/images/game_bg.png")) {
            std::cerr << "Warning: Could not load game_bg.png" << std::endl;
        }

        // Load fonts
        std::vector<std::string> fontPaths = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "/Windows/Fonts/arial.ttf",
            "arial.ttf"
        };

        bool fontLoaded = false;
        for (const auto& path : fontPaths) {
            if (loadFont("main", path)) {
                fontLoaded = true;
                break;
            }
        }

        return fontLoaded;
    }
};

} // namespace gui
} // namespace coup