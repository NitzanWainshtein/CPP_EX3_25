// Email: nitzanwa@gmail.com

#include <iostream>
#include "GameGUI.hpp"

using namespace std;
using namespace coup;

int main() {
    try {
        cout << "🎮 Starting Coup GUI..." << endl;

        CoupGUI gui;
        gui.run();

        cout << "👋 Thanks for playing!" << endl;
        return 0;

    } catch (const exception& e) {
        cerr << "❌ GUI Error: " << e.what() << endl;
        return 1;
    }
}