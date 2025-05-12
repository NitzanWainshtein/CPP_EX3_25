#include "Game.hpp"
#include "Player.hpp"
#include "Players/Roles/Governor.hpp"
#include "Players/Roles/Spy.hpp"
#include "Players/Roles/Baron.hpp"
#include "Players/Roles/Merchant.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace coup;
using namespace std;

Player* findValidTarget(const Game& game, Player* self, const vector<Player*>& allPlayers) {
    for (Player* p : allPlayers) {
        if (p != self && game.isAlive(*p)) {
            return p;
        }
    }
    return nullptr;
}

void printStatus(const Game& game, const vector<Player*>& players) {
    cout << "\n[STATUS] Players still in game:" << endl;
    for (Player* p : players) {
        if (game.isAlive(*p)) {
            cout << " - " << p->getName() << " (" << p->getRoleName() << ") : " << p->getCoins() << " coins" << endl;
        }
    }
    cout << endl;
}

int main() {
    Game game;

    Governor gov(game, "Alice");
    Spy spy(game, "Bob");
    Baron baron(game, "Charlie");
    Merchant merchant(game, "Diana");

    vector<Player*> allPlayers = {&gov, &spy, &baron, &merchant};
    unordered_map<string, int> playerTurnCount;

    for (Player* p : allPlayers) {
        playerTurnCount[p->getName()] = 0;
    }

    cout << "\n=== Starting Full Game Test Simulation ===\n" << endl;

    while (!game.isGameOver()) {
        string currentName = game.turn();
        Player* current = nullptr;
        for (Player* p : allPlayers) {
            if (p->getName() == currentName) {
                current = p;
                break;
            }
        }
        if (!current) {
            cout << "[BUG] Could not match current turn player to any known pointer." << endl;
            break;
        }

        bool extraTurn = false;

        do {
            cout << "\n[TURN] " << currentName << " (" << current->getRoleName() << ") begins" << endl;

            try {
                current->startTurn();

                if (!extraTurn) playerTurnCount[currentName]++;
                int turnNum = playerTurnCount[currentName];

                if (game.hasPendingAction()) {
                    if (current->getRoleName() == "Governor") {
                        Player* actor = game.getLastActor();
                        if (actor && actor->getLastAction() == ActionType::Tax) {
                            cout << "[ACTION] Governor attempts to undo " << actor->getName() << "'s tax." << endl;
                            current->undo(*actor);
                        }
                    }
                }

                Player* target = findValidTarget(game, current, allPlayers);

                if (current->getCoins() >= 10) {
                    if (target) {
                        cout << "[ACTION] " << current->getName() << " performs COUP on " << target->getName() << endl;
                        current->coup(*target);
                    } else {
                        cout << "[BUG] " << current->getName() << " must coup but no valid targets available!" << endl;
                    }
                } else if (current->getName() == "Alice") { // Governor
                    cout << "[ACTION] Alice uses TAX." << endl;
                    current->tax();
                } else if (current->getName() == "Bob") { // Spy
                    if (turnNum == 2 && game.isAlive(baron)) {
                        cout << "[ACTION] Spy blocks Baron's next arrest." << endl;
                        spy.blockNextArrest(baron);
                    } else if (turnNum == 3 && game.isAlive(baron)) {
                        cout << "[ACTION] Spy peeks Baron: " << spy.peekCoins(baron) << " coins." << endl;
                    } else if (current->getCoins() >= 4) {
                        cout << "[ACTION] Spy uses BRIBE." << endl;
                        current->bribe();
                        extraTurn = true;
                        continue; // חוזר לתור נוסף מיד
                    } else {
                        cout << "[ACTION] Spy gathers coins." << endl;
                        current->gather();
                    }
                } else if (current->getName() == "Charlie") { // Baron
                    if (turnNum == 4 && game.isAlive(merchant) && merchant.getCoins() > 0) {
                        cout << "[ACTION] Baron attempts to ARREST Merchant." << endl;
                        try {
                            baron.arrest(merchant);
                        } catch (const exception& e) {
                            cout << "[EXPECTED ERROR] Baron arrest failed: " << e.what() << endl;
                            baron.gather();
                        }
                    } else if (turnNum == 5 && game.isAlive(merchant)) {
                        cout << "[ACTION] Baron attempts to ARREST Merchant again." << endl;
                        baron.arrest(merchant); // arrest נוסף שמצליח
                    } else if (current->getCoins() >= 3) {
                        cout << "[ACTION] Baron invests (3 coins → 6 coins)." << endl;
                        baron.invest();
                    } else {
                        cout << "[ACTION] Baron gathers coins." << endl;
                        current->gather();
                    }
                } else if (current->getName() == "Diana") { // Merchant
                    if (turnNum == 2 && game.isAlive(baron)) {
                        if (current->getCoins() >= 3) {
                            cout << "[ACTION] Merchant sanctions Baron." << endl;
                            current->sanction(baron);
                        } else {
                            cout << "[ACTION] Merchant can't sanction (not enough coins). Gathering instead." << endl;
                            current->gather();
                        }
                    } else {
                        cout << "[ACTION] Merchant gathers coins." << endl;
                        current->gather();
                    }
                } else {
                    cout << "[BUG] Unknown player encountered." << endl;
                }

                extraTurn = false;
                current->endTurn();
                printStatus(game, allPlayers);

            } catch (const exception& e) {
                cout << "[ERROR] Exception in turn for " << currentName << ": " << e.what() << endl;
                try {
                    current->endTurn();
                } catch (...) {
                    cout << "[BUG] Failed to end turn after error." << endl;
                }
                break;
            }
        } while (extraTurn);
    }

    cout << "\n=== Game Over ===\n" << endl;
    try {
        cout << "Winner: " << game.winner() << endl;
    } catch (const exception& e) {
        cout << "[BUG] Could not determine winner: " << e.what() << endl;
    }

    return 0;
}
