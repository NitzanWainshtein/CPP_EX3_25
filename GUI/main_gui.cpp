// GUI/main.cpp
#include "CoupGUI.hpp"
#include <iostream>

int main() {
    try {
        std::cout << "🎮 Starting Coup GUI..." << std::endl;

        coup::gui::CoupGUI gui;

        if (!gui.initialize()) {
            std::cerr << "Failed to initialize GUI!" << std::endl;
            return 1;
        }

        gui.run();

        std::cout << "👋 Thanks for playing!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }
}