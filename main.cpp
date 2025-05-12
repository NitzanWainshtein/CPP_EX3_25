#include "GameLogic/Game.hpp"
#include "Players/Player.hpp"
#include "Players/Roles/Governor.hpp"
#include "Players/Roles/Spy.hpp"
#include "Players/Roles/Baron.hpp"
#include "Players/Roles/Merchant.hpp"
#include "Players/Roles/General.hpp"
#include "Players/Roles/Judge.hpp"

#include <iostream>
#include <vector>
#include <string>

using namespace coup;
using namespace std;

// Helper function to verify if a player is active
bool isActivePlayer(const Game& game, const string& playerName) {
    vector<string> activePlayers = game.players();
    for (const string& name : activePlayers) {
        if (name == playerName) {
            return true;
        }
    }
    return false;
}

// Helper function to print the game state
void printGameState(Game& game, const vector<Player*>& players) {
    cout << "\n--- Current Game State ---" << endl;
    cout << "Current turn: " << game.turn() << endl;

    vector<string> activePlayers = game.players();
    cout << "Active players: ";
    for (const string& name : activePlayers) {
        cout << name << " ";
    }
    cout << endl;

    cout << "Player details:" << endl;
    for (Player* player : players) {
        if (player == nullptr) continue;

        // Check if this player is active in the game
        bool isActive = isActivePlayer(game, player->getName());

        if (isActive) {
            cout << " - " << player->getName() << " (" << player->getRoleName()
                 << "): " << player->getCoins() << " coins";
            if (player->isSanctioned()) cout << " [SANCTIONED]";
            cout << endl;
        }
    }
    cout << "-------------------------\n" << endl;
}

// Helper function to find a player by name
Player* findPlayer(const string& name, const vector<Player*>& players) {
    for (Player* player : players) {
        if (player != nullptr && player->getName() == name) {
            return player;
        }
    }
    return nullptr;
}

// Improved helper function for basic player turn (for gathering or simple actions)
void basicTurn(Game& game, Player* player) {
    try {
        player->gather();
    } catch (const exception& e) {
        cout << "  Error: " << e.what() << endl;
        // Directly advance turn if sanctioned
        if (player->isSanctioned()) {
            cout << "  Player is sanctioned, manually advancing turn" << endl;
            // Clear the sanction flag and advance the turn
            player->setSanctioned(false);
            game.nextTurn();
        }
    }
}

// Helper function to ensure it's a specific player's turn
void ensureTurn(Game& game, const string& playerName, vector<Player*>& players) {
    int safetyCounter = 0; // Prevent infinite loops

    while (game.turn() != playerName && !game.isGameOver() && safetyCounter < 15) {
        safetyCounter++;
        string currentTurn = game.turn();
        cout << "Advancing turn from " << currentTurn << " to " << playerName << "..." << endl;

        // Find the current player and make them gather to advance turn
        Player* currentPlayer = findPlayer(currentTurn, players);
        if (currentPlayer) {
            // If player is sanctioned, we can't use gather. Let's complete their turn directly
            basicTurn(game, currentPlayer);
        } else {
            cout << "Error: Current player not found!" << endl;
            return;
        }
    }

    if (safetyCounter >= 15) {
        cout << "WARNING: Maximum turn iterations reached. Breaking potential infinite loop." << endl;
        Player* currentPlayer = findPlayer(game.turn(), players);
        if (currentPlayer && currentPlayer->isSanctioned()) {
            cout << "  Force clearing sanction and advancing turn" << endl;
            currentPlayer->setSanctioned(false);
            game.nextTurn();
        }
    }
}

// Helper function to test a simple game feature
void testFeature(const string& testName, Game& game, vector<Player*>& players, function<void()> testFunction) {
    cout << "\n===== " << testName << " =====" << endl;

    try {
        testFunction();
    } catch (const exception& e) {
        cout << "ERROR in " << testName << ": " << e.what() << endl;
        // Try to recover - get current player and advance turn
        string currentTurn = game.turn();
        Player* currentPlayer = findPlayer(currentTurn, players);
        if (currentPlayer && currentPlayer->isSanctioned()) {
            cout << "  Recovering from error - clearing sanction and advancing turn" << endl;
            currentPlayer->setSanctioned(false);
            game.nextTurn();
        }
    }

    printGameState(game, players);
}

// Add this to Player.hpp:
// void setSanctioned(bool value) { sanctioned = value; }
// Or modify directly in the Test:
namespace coup {
    // Extension to Player class for test access
    void Player::setSanctioned(bool value) {
        sanctioned = value;
    }
}

int main() {
    cout << "=== Starting Basic Coup Game ===\n" << endl;

    // Create game
    Game game;

    // Create players
    Governor governor(game, "Moshe");
    Spy spy(game, "Yossi");
    Baron baron(game, "Meirav");
    General general(game, "Reut");
    Judge judge(game, "Gilad");
    Merchant merchant(game, "David");

    // Store players for easier access
    vector<Player*> players = {&governor, &spy, &baron, &general, &judge, &merchant};

    // Initial state
    cout << "Initial game setup:" << endl;
    printGameState(game, players);

    // Build up coins
    cout << "Building up coins for all players..." << endl;
    for (int round = 1; round <= 3; round++) {
        for (Player* player : players) {
            if (isActivePlayer(game, player->getName())) {
                ensureTurn(game, player->getName(), players);
                basicTurn(game, player);
            }
        }
    }

    printGameState(game, players);

    // Test 1: Governor's Tax Ability
    testFeature("Governor's Special Tax", game, players, [&]() {
        ensureTurn(game, "Moshe", players);
        int beforeCoins = governor.getCoins();
        cout << "Governor has " << beforeCoins << " coins before tax." << endl;
        governor.tax();
        cout << "Governor now has " << governor.getCoins() << " coins after tax (expected: +" << governor.taxAmount() << ")." << endl;
    });

    // Test 2: Spy's Abilities
    testFeature("Spy's Special Abilities", game, players, [&]() {
        ensureTurn(game, "Yossi", players);
        cout << "Spy peeks at Baron's coins: " << spy.peekCoins(baron) << endl;
        cout << "Spy blocks Governor's next arrest." << endl;
        spy.blockNextArrest(governor);
        spy.gather();

        // Test if Governor can arrest after being blocked
        ensureTurn(game, "Moshe", players);
        try {
            cout << "Governor tries to arrest Spy (should fail)." << endl;
            governor.arrest(spy);
            cout << "ERROR: Governor successfully arrested Spy despite being blocked!" << endl;
        } catch (const exception& e) {
            cout << "Expected error: " << e.what() << endl;
            governor.gather(); // End governor's turn instead
        }
    });

    // Test 3: Baron's Invest Ability
    testFeature("Baron's Invest Ability", game, players, [&]() {
        // Make sure Baron has 3+ coins
        ensureTurn(game, "Meirav", players);
        if (baron.getCoins() < 3) {
            cout << "Baron needs more coins before testing invest." << endl;
            baron.gather();
            ensureTurn(game, "Meirav", players);
        }

        int beforeCoins = baron.getCoins();
        cout << "Baron has " << beforeCoins << " coins before investing." << endl;
        baron.invest();
        cout << "Baron now has " << baron.getCoins() << " coins after investing (expected: invested 3, received 6)." << endl;
    });

    // Test 4: General's Recovery from Arrest
    testFeature("General's Recovery from Arrest", game, players, [&]() {
        ensureTurn(game, "Yossi", players);
        int beforeGenCoins = general.getCoins();
        cout << "General has " << beforeGenCoins << " coins before arrest." << endl;

        spy.arrest(general);
        cout << "Spy arrested General." << endl;

        ensureTurn(game, "Reut", players);
        cout << "General now has " << general.getCoins() << " coins after arrest." << endl;

        // General should've recovered the coin
        if (general.getCoins() == beforeGenCoins) {
            cout << "General successfully recovered the coin after arrest!" << endl;
        } else {
            cout << "General did NOT recover the coin after arrest." << endl;
        }

        general.gather();
    });

    // Build up more coins for final tests
    cout << "Building up more coins..." << endl;
    for (int round = 1; round <= 2; round++) {
        for (Player* player : players) {
            if (isActivePlayer(game, player->getName())) {
                ensureTurn(game, player->getName(), players);
                basicTurn(game, player);
            }
        }
    }

    printGameState(game, players);

    // Test 5: Coup Mechanics
    testFeature("Final Coup Test", game, players, [&]() {
        // Look for a player with 7+ coins
        for (Player* player : players) {
            if (isActivePlayer(game, player->getName()) && player->getCoins() >= 7) {
                ensureTurn(game, player->getName(), players);

                // Find a target
                for (Player* target : players) {
                    if (target != player && isActivePlayer(game, target->getName())) {
                        cout << player->getName() << " performs coup on " << target->getName() << "!" << endl;
                        player->coup(*target);
                        break;
                    }
                }
                break;
            }
        }
    });

    // Final game state
    cout << "\n=== Final Game State ===\n" << endl;
    printGameState(game, players);

    return 0;
}