#include "GameLogic/Game.hpp"
#include "Players/Roles/Governor.hpp"
#include <iostream>

using namespace std;
using namespace coup;

int main() {
    try {
        Game game;

        Governor g1(game, "Alice");
        Governor g2(game, "Bob");

        cout << "--- Game Start ---\n";
        cout << "Initial Bank: " << game.getBank() << " coins\n";

        cout << "Registered players in order: ";
        for (const auto &name : game.players()) {
            cout << name << " ";
        }
        cout << "\nStarting turn belongs to: " << game.turn() << "\n";

        // Round 1
        cout << "\n=== Round 1 ===\n";
        try {
            g1.startTurn(); g1.gather(); g1.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g1.coup(g2); g1.endTurn();
        }

        try {
            g2.startTurn(); g2.gather(); g2.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g2.coup(g1); g2.endTurn();
        }

        // Round 2
        cout << "\n=== Round 2 ===\n";
        try {
            g1.startTurn(); g1.gather(); g1.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g1.coup(g2); g1.endTurn();
        }

        try {
            g2.startTurn(); g2.gather(); g2.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g2.coup(g1); g2.endTurn();
        }

        // Round 3
        cout << "\n=== Round 3 ===\n";
        try {
            g1.startTurn(); g1.gather(); g1.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g1.coup(g2); g1.endTurn();
        }

        try {
            g2.startTurn(); g2.tax(); g2.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g2.coup(g1); g2.endTurn();
        }

        // Round 4
        cout << "\n=== Round 4 ===\n";
        try {
            g1.startTurn(); g1.tax(); g1.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g1.coup(g2); g1.endTurn();
        }

        try {
            g2.startTurn(); g2.gather(); g2.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g2.coup(g1); g2.endTurn();
        }

        // Round 5
        cout << "\n=== Round 5 ===\n";
        try {
            g1.startTurn(); g1.gather(); g1.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g1.coup(g2); g1.endTurn();
        }

        try {
            g2.startTurn(); g2.gather(); g2.bribe(); g2.startTurn(); g2.tax(); g2.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g2.coup(g1); g2.endTurn();
        }

        // Round 6
        cout << "\n=== Round 6 ===\n";
        try {
            g1.startTurn(); g1.tax(); g1.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g1.coup(g2); g1.endTurn();
        }

        // Round 7
        cout << "\n=== Round 7 ===\n";
        try {
            g2.startTurn(); g2.undo(g1); g2.gather(); g2.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g2.coup(g1); g2.endTurn();
        }

        // Round 8
        cout << "\n=== Round 8 ===\n";
        try {
            g1.startTurn();
            if (g1.getCoins() >= 10) {
                g1.coup(g2);
                cout << g1.getName() << " performs coup on " << g2.getName() << "!\n";
            }
            g1.endTurn();
        } catch (const runtime_error &e) {
            cout << e.what() << "\n";
            g1.coup(g2); g1.endTurn();
        }

        // Check winner
        if (game.isGameOver()) {
            cout << "\n*** Winner: " << game.winner() << " ***\n";
        }

        cout << "\n--- Game End ---\n";
        cout << g1.getName() << ": " << g1.getCoins() << " coins\n";
        cout << g2.getName() << ": " << g2.getCoins() << " coins\n";
        cout << "Bank: " << game.getBank() << " coins\n";

        vector<string> remaining = game.players();
        cout << "Remaining players (" << remaining.size() << "): ";
        for (const auto &name : remaining) {
            cout << name << " ";
        }
        cout << endl;

        if (game.isGameOver()) {
            cout << "\n*** Winner: " << game.winner() << " ***\n";
        } else if (remaining.empty()) {
            cout << "\n*** No players remaining. Game ended unexpectedly. ***\n";
        }

    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
