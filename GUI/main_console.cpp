// Email: nitzanwa@gmail.com

#include <iostream>
#include <string>
#include <limits>
#include "../GameLogic/Game.hpp"
#include "../GameLogic/PlayerFactory.hpp"
#include "../Players/Roles/Governor.hpp"
#include "../Players/Roles/Spy.hpp"
#include "../Players/Roles/Baron.hpp"
#include "../Players/Roles/General.hpp"
#include "../Players/Roles/Judge.hpp"
#include "../Players/Roles/Merchant.hpp"

using namespace std;
using namespace coup;

void printWelcome() {
    cout << "\n" << string(60, '=') << endl;
    cout << "🎮              COUP GAME LAUNCHER              🎮" << endl;
    cout << string(60, '=') << endl;
    cout << "\nChoose your version:" << endl;
    cout << "1. Simple Demo" << endl;
    cout << "2. Exit" << endl;
    cout << "\nEnter choice (1-2): ";
}

int runSimpleDemo() {
    try {
        cout << "🎬 Running simple demo..." << endl;

        Game game;

        // Create players using the correct approach
        Governor* governor = new Governor(game, "Alice");
        Spy* spy = new Spy(game, "Bob");
        Baron* baron = new Baron(game, "Charlie");
        General* general = new General(game, "Diana");
        Judge* judge = new Judge(game, "Eve");

        // Set initial coins
        governor->setCoins(2);
        spy->setCoins(1);
        baron->setCoins(3);
        general->setCoins(2);
        judge->setCoins(1);

        cout << "\n📊 Initial state:" << endl;
        auto players = game.players();
        for (const auto& name : players) {
            cout << "- " << name << endl;
        }

        cout << "\nCurrent turn: " << game.turn() << endl;

        // Simple actions
        cout << "\n🎯 Performing some actions..." << endl;

        try {
            governor->gather();
            cout << "✅ Governor gathered coins (now has " << governor->getCoins() << ")" << endl;

            spy->gather();
            cout << "✅ Spy gathered coins (now has " << spy->getCoins() << ")" << endl;

            baron->invest();
            cout << "✅ Baron invested (now has " << baron->getCoins() << ")" << endl;

            general->gather();
            cout << "✅ General gathered coins (now has " << general->getCoins() << ")" << endl;

            judge->gather();
            cout << "✅ Judge gathered coins (now has " << judge->getCoins() << ")" << endl;

        } catch (const exception& e) {
            cout << "⚠️ Action failed: " << e.what() << endl;
        }

        // Clean up
        delete governor;
        delete spy;
        delete baron;
        delete general;
        delete judge;

        cout << "\n🏁 Demo completed successfully!" << endl;
        return 0;

    } catch (const exception& e) {
        cout << "❌ Demo Error: " << e.what() << endl;
        return 1;
    }
}

int main() {
    int choice;

    while (true) {
        printWelcome();

        if (!(cin >> choice)) {
            cout << "❌ Invalid input! Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                cout << "\n🎬 Starting Demo Mode..." << endl;
                return runSimpleDemo();

            case 2:
                cout << "\n👋 Thanks for playing! Goodbye!" << endl;
                return 0;

            default:
                cout << "❌ Invalid choice! Please enter 1 or 2." << endl;
                break;
        }
    }
}