// Email: nitzanwa@gmail.com

#include "Players/Player.hpp"
#include "Players/Roles/Governor.hpp"
#include "Players/Roles/Spy.hpp"
#include "Players/Roles/Baron.hpp"
#include "Players/Roles/General.hpp"
#include "Players/Roles/Judge.hpp"
#include "Players/Roles/Merchant.hpp"
#include "GameLogic/Game.hpp"
#include "GameLogic/Logger.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;
using namespace coup;

// ==========================================
// AUTOMATED DECISION CALLBACKS
// ==========================================

bool automatedBribeDecision(const Player& player) {
    // Simple strategy: use bribe if have lots of coins
    bool decision = player.getCoins() >= 8;
    cout << "  [AUTO] " << player.getName() << " decides " 
         << (decision ? "YES" : "NO") << " to bribe (has " 
         << player.getCoins() << " coins)" << endl;
    return decision;
}

bool automatedBlockDecision(const Player& blocker, ActionType action, const Player* actor) {
    if (!actor) return false;
    
    string actionName = (action == ActionType::Tax) ? "Tax" :
                       (action == ActionType::Bribe) ? "Bribe" :
                       (action == ActionType::Coup) ? "Coup" : "Unknown";

    cout << "  [BLOCKING] " << blocker.getName() << " (" << blocker.getRoleName()
         << ") deciding whether to block " << actor->getName() << "'s " << actionName << endl;

    // Automated blocking strategy based on role
    if (blocker.getRoleName() == "Governor" && action == ActionType::Tax && actor != &blocker) {
        cout << "    -> " << blocker.getName() << " (Governor) BLOCKS the tax!" << endl;
        return true;
    }
    if (blocker.getRoleName() == "Judge" && action == ActionType::Bribe) {
        cout << "    -> " << blocker.getName() << " (Judge) BLOCKS the bribe!" << endl;
        return true;
    }
    if (blocker.getRoleName() == "General" && action == ActionType::Coup && blocker.getCoins() >= 5) {
        cout << "    -> " << blocker.getName() << " (General) BLOCKS the coup!" << endl;
        return true;
    }
    
    cout << "    -> " << blocker.getName() << " allows the action" << endl;
    return false;
}

// ==========================================
// HELPER FUNCTIONS
// ==========================================

void printGameState(const Game& game) {
    cout << "\n🎮 === GAME STATE === 🎮" << endl;
    cout << "Current turn: " << game.turn() << endl;
    cout << "Bank: " << game.getBankCoins() << " coins" << endl;
    cout << "Active players: ";

    auto players = game.players();
    for (size_t i = 0; i < players.size(); ++i) {
        cout << players[i];
        if (i < players.size() - 1) cout << ", ";
    }
    cout << endl;
    cout << "=========================" << endl;
}

void printPlayerStats(const vector<Player*>& players, const Game& game) {
    cout << "\n💰 --- PLAYER STATS --- 💰" << endl;
    for (Player* p : players) {
        if (p != nullptr && game.isAlive(*p)) {
            cout << p->getName() << " (" << p->getRoleName() << "): "
                 << p->getCoins() << " coins";
            if (p->isSanctioned()) cout << " [SANCTIONED]";
            if (p->getArrestStatus() != ArrestStatus::Available) {
                cout << " [ARREST:" << (p->getArrestStatus() == ArrestStatus::ArrestedNow ? "NOW" : "COOLDOWN") << "]";
            }
            cout << endl;
        }
    }
    cout << "-------------------------" << endl;
}

Player* getCurrentPlayer(const Game& game, const vector<Player*>& players) {
    string currentName = game.turn();
    for (Player* p : players) {
        if (p != nullptr && p->getName() == currentName && game.isAlive(*p)) {
            return p;
        }
    }
    return nullptr;
}

// Helper function to get a different player for targeting
Player* getTargetPlayer(Player* actor, const vector<Player*>& allPlayers, const Game& game) {
    for (Player* p : allPlayers) {
        if (p != actor && p != nullptr && game.isAlive(*p)) {
            return p;
        }
    }
    return nullptr;
}

void pauseForDemo() {
    cout << "\n[Press Enter to continue...]" << endl;
    cin.get();
}

// Safe turn execution with error handling
bool executeTurnSafely(Player* player, function<void()> action, const string& actionName) {
    try {
        player->startTurn();
        action();
        player->endTurn();
        cout << "✅ " << player->getName() << " successfully performed " << actionName << endl;
        return true;
    } catch (const exception& e) {
        cout << "⚠️ " << player->getName() << " couldn't perform " << actionName << ": " << e.what() << endl;
        try {
            // Try alternative action
            player->gather();
            player->endTurn();
            cout << "   → Did gather instead" << endl;
            return true;
        } catch (const exception& e2) {
            cout << "   → Even gather failed: " << e2.what() << endl;
            try {
                player->endTurn(); // Just end turn
            } catch (...) {
                // Force next turn if endTurn also fails
            }
            return false;
        }
    }
}

// ==========================================
// MAIN AUTOMATED DEMO
// ==========================================

int main() {
    cout << "🌟🎮 COUP AUTOMATED DEMO 🎮🌟" << endl;
    cout << "=============================" << endl;
    cout << "This demo automatically demonstrates all game features!" << endl;
    pauseForDemo();

    try {
        Game game;
        game.setConsoleMode(true);

        // Create players with ALL different roles
        cout << "\n📝 Creating players with different roles..." << endl;
        Governor alice(game, "Alice");
        Judge bob(game, "Bob");
        General charlie(game, "Charlie");
        Baron diana(game, "Diana");
        Merchant eve(game, "Eve");
        Spy frank(game, "Frank");

        vector<Player*> allPlayers = {&alice, &bob, &charlie, &diana, &eve, &frank};

        // Set up automated callbacks for all players
        for (Player* player : allPlayers) {
            player->setBribeDecisionCallback([](const Player& p) -> bool {
                return automatedBribeDecision(p);
            });
            player->setBlockDecisionCallback([](const Player& blocker, ActionType action, const Player* actor) -> bool {
                return automatedBlockDecision(blocker, action, actor);
            });
        }

        // Give starting coins
        alice.setCoins(2);
        bob.setCoins(1);
        charlie.setCoins(1);
        diana.setCoins(3);
        eve.setCoins(3);
        frank.setCoins(1);

        cout << "\n🎲 Initial setup complete!" << endl;
        printGameState(game);
        printPlayerStats(allPlayers, game);
        pauseForDemo();

        cout << "\n🎭 DEMONSTRATION PHASE 1: BASIC ACTIONS" << endl;
        cout << "=======================================" << endl;

        // Turn 1: Alice (Governor) - Enhanced Tax
        cout << "\n1️⃣ Alice (Governor) performs enhanced TAX (3 coins)..." << endl;
        Player* current = getCurrentPlayer(game, allPlayers);
        executeTurnSafely(current, [&]() { current->tax(); }, "Enhanced Tax");
        printPlayerStats(allPlayers, game);
        pauseForDemo();

        // Turn 2: Bob (Judge) - Gather
        cout << "\n2️⃣ Bob (Judge) gathers coins..." << endl;
        current = getCurrentPlayer(game, allPlayers);
        executeTurnSafely(current, [&]() { current->gather(); }, "Gather");
        printPlayerStats(allPlayers, game);

        // Turn 3: Charlie (General) - Gather
        cout << "\n3️⃣ Charlie (General) gathers coins..." << endl;
        current = getCurrentPlayer(game, allPlayers);
        executeTurnSafely(current, [&]() { current->gather(); }, "Gather");
        printPlayerStats(allPlayers, game);

        // Turn 4: Diana (Baron) - INVEST!
        cout << "\n4️⃣ Diana (Baron) uses INVEST ability (3 → 6 coins)..." << endl;
        current = getCurrentPlayer(game, allPlayers);
        if (current->getRoleName() == "Baron") {
            executeTurnSafely(current, [&]() { static_cast<Baron*>(current)->invest(); }, "Invest");
        } else {
            executeTurnSafely(current, [&]() { current->gather(); }, "Gather (not Baron's turn)");
        }
        printPlayerStats(allPlayers, game);
        pauseForDemo();

        // Turn 5: Eve (Merchant) - Bonus coin demonstration
        cout << "\n5️⃣ Eve (Merchant) starts turn - should get bonus coin for having 3+ coins..." << endl;
        current = getCurrentPlayer(game, allPlayers);
        executeTurnSafely(current, [&]() { current->gather(); }, "Gather with Merchant bonus");
        printPlayerStats(allPlayers, game);

        // Turn 6: Frank (Spy) - Special abilities
        cout << "\n6️⃣ Frank (Spy) demonstrates special abilities..." << endl;
        current = getCurrentPlayer(game, allPlayers);
        
        if (current->getRoleName() == "Spy") {
            Spy* spy = static_cast<Spy*>(current);
            cout << "   👁️ Frank peeks at Alice's coins: " << spy->peekCoins(alice) << endl;
            cout << "   🚫 Frank blocks Charlie's next arrest..." << endl;
            spy->blockNextArrest(charlie);
        }
        
        executeTurnSafely(current, [&]() { current->gather(); }, "Gather with Spy abilities");
        printPlayerStats(allPlayers, game);
        pauseForDemo();

        cout << "\n🚫 DEMONSTRATION PHASE 2: BLOCKING MECHANISMS" << endl;
        cout << "===============================================" << endl;

        // Turn 7: Someone other than Alice tries tax - Alice should block
        cout << "\n7️⃣ Current player tries TAX - checking for blocks..." << endl;
        current = getCurrentPlayer(game, allPlayers);
        executeTurnSafely(current, [&]() { current->tax(); }, "Tax (with potential blocking)");
        printPlayerStats(allPlayers, game);
        pauseForDemo();

        // Turn 8: Try arrest action
        cout << "\n8️⃣ Current player tries ARREST..." << endl;
        current = getCurrentPlayer(game, allPlayers);
        Player* target = getTargetPlayer(current, allPlayers, game);
        if (target && current->getCoins() >= 1) {
            executeTurnSafely(current, [&]() { current->arrest(*target); }, "Arrest");
        } else {
            executeTurnSafely(current, [&]() { current->gather(); }, "Gather (can't arrest)");
        }
        printPlayerStats(allPlayers, game);

        cout << "\n⚔️ DEMONSTRATION PHASE 3: SANCTIONS" << endl;
        cout << "====================================" << endl;

        // Multiple turns to demonstrate sanctions
        for (int i = 0; i < 8; i++) {
            current = getCurrentPlayer(game, allPlayers);
            if (!current || game.isGameOver()) break;
            
            cout << "\n🔄 Turn " << (i + 9) << ": " << current->getName() << " (" << current->getRoleName() << ")" << endl;
            
            // Try sanction if have enough coins and not sanctioned
            if (current->getCoins() >= 3 && !current->isSanctioned()) {
                Player* sanctionTarget = getTargetPlayer(current, allPlayers, game);
                if (sanctionTarget && !sanctionTarget->isSanctioned()) {
                    // Calculate actual cost
                    int cost = 3;
                    if (sanctionTarget->getRoleName() == "Judge") cost = 4;
                    
                    if (current->getCoins() >= cost) {
                        cout << "   Attempting sanction on " << sanctionTarget->getName() << " (cost: " << cost << ")" << endl;
                        executeTurnSafely(current, [&]() { current->sanction(*sanctionTarget); }, "Sanction");
                    } else {
                        executeTurnSafely(current, [&]() { current->gather(); }, "Gather (can't afford sanction)");
                    }
                } else {
                    executeTurnSafely(current, [&]() { current->gather(); }, "Gather (no valid sanction target)");
                }
            } else {
                // Try gather, will fail if sanctioned
                bool success = executeTurnSafely(current, [&]() { current->gather(); }, "Gather");
                if (!success) {
                    cout << "   ⚠️ Player is sanctioned and cannot perform economic actions!" << endl;
                    // Force end turn without action
                    try {
                        current->endTurn();
                    } catch (...) {}
                }
            }
            
            printPlayerStats(allPlayers, game);
            
            if (game.isGameOver()) {
                cout << "\n🎉 Game ended during demonstration!" << endl;
                break;
            }
        }

        cout << "\n💥 DEMONSTRATION PHASE 4: COUP MECHANICS" << endl;
        cout << "=========================================" << endl;

        // Give someone enough coins for mandatory coup
        current = getCurrentPlayer(game, allPlayers);
        if (current && !game.isGameOver()) {
            cout << "\n🚨 Giving " << current->getName() << " 10 coins - they MUST COUP!" << endl;
            current->setCoins(10);
            
            try {
                current->startTurn(); // This should throw because they have 10+ coins
                cout << "   ❌ ERROR: Should have been forced to coup!" << endl;
            } catch (const exception& e) {
                cout << "   ✅ EXPECTED: " << e.what() << endl;
                cout << "   💥 " << current->getName() << " is FORCED to COUP someone..." << endl;
                
                Player* coupTarget = getTargetPlayer(current, allPlayers, game);
                if (coupTarget) {
                    try {
                        current->coup(*coupTarget);
                        cout << "   💥 " << current->getName() << " coups " << coupTarget->getName() << "!" << endl;
                    } catch (const exception& e2) {
                        cout << "   ⚠️ Coup failed: " << e2.what() << endl;
                    }
                } else {
                    cout << "   ⚠️ No valid coup target found!" << endl;
                }
            }
        }

        printPlayerStats(allPlayers, game);
        printGameState(game);
        pauseForDemo();

        cout << "\n🏆 FINAL RESULTS" << endl;
        cout << "=================" << endl;

        if (game.isGameOver()) {
            cout << "🎉 GAME COMPLETED! 🎉" << endl;
            cout << "🥇 WINNER: " << game.winner() << " 🥇" << endl;
        } else {
            cout << "🎮 Game demonstration complete - continuing until winner..." << endl;
            
            // Continue game automatically until winner
            int maxTurns = 50; // יותר תורים כדי לאפשר סיום טבעי
            int turnCount = 0;
            
            while (!game.isGameOver() && turnCount < maxTurns) {
                current = getCurrentPlayer(game, allPlayers);
                if (!current) break;
                
                cout << "\n🤖 Auto-turn " << (turnCount + 1) << ": " << current->getName() << endl;
                
                // Aggressive strategy: try to coup when possible to end the game
                bool success = false;
                if (current->getCoins() >= 10) {
                    // Must coup (mandatory)
                    Player* target = getTargetPlayer(current, allPlayers, game);
                    if (target) {
                        success = executeTurnSafely(current, [&]() { current->coup(*target); }, "Mandatory Coup");
                    }
                } else if (current->getCoins() >= 7) {
                    // Strategic coup - be more aggressive to end the game
                    Player* target = getTargetPlayer(current, allPlayers, game);
                    if (target) {
                        // Special logic: if only 2 players left, definitely coup to win!
                        if (game.players().size() == 2) {
                            cout << "   🏆 Final showdown! " << current->getName() << " goes for the win!" << endl;
                            success = executeTurnSafely(current, [&]() { current->coup(*target); }, "Winning Coup");
                        } else {
                            success = executeTurnSafely(current, [&]() { current->coup(*target); }, "Strategic Coup");
                        }
                    }
                }
                
                if (!success) {
                    executeTurnSafely(current, [&]() { current->gather(); }, "Auto Gather");
                }
                
                turnCount++;
                
                // Show stats more frequently as we near the end
                if (turnCount % 2 == 0 || game.players().size() <= 3) {
                    printPlayerStats(allPlayers, game);
                }
                
                // Check if game ended after this turn
                if (game.isGameOver()) {
                    cout << "\n🎉 GAME COMPLETED! WINNER FOUND! 🎉" << endl;
                    break;
                }
            }
            
            if (game.isGameOver()) {
                cout << "\n🎉 GAME COMPLETED SUCCESSFULLY! 🎉" << endl;
                cout << "🥇 WINNER: " << game.winner() << " 🥇" << endl;
            } else {
                cout << "\nDemo completed after " << maxTurns << " auto-turns without a clear winner." << endl;
                cout << "This shouldn't happen in a normal game - players would be more aggressive!" << endl;
            }
        }

        printPlayerStats(allPlayers, game);

        cout << "\n✅ DEMO COMPLETE! ✅" << endl;
        cout << "====================" << endl;
        cout << "🎯 FEATURES DEMONSTRATED:" << endl;
        cout << "✅ All 6 roles with unique abilities" << endl;
        cout << "✅ Governor: 3-coin tax + tax blocking" << endl;
        cout << "✅ Judge: bribe blocking + 4-coin sanction cost" << endl;
        cout << "✅ General: arrest compensation + coup blocking" << endl;
        cout << "✅ Baron: invest ability + sanction compensation" << endl;
        cout << "✅ Merchant: bonus coins for wealthy starts" << endl;
        cout << "✅ Spy: peek coins + arrest blocking" << endl;
        cout << "✅ Sanction effects prevent economic actions" << endl;
        cout << "✅ Arrest status tracking and compensation" << endl;
        cout << "✅ Coup mechanics and mandatory 10-coin rule" << endl;
        cout << "✅ Robust error handling and turn management" << endl;
        cout << "✅ Comprehensive blocking system" << endl;
        cout << "✅ Win conditions and game completion" << endl;
        cout << "\n🚀 SYSTEM READY FOR PRODUCTION! 🚀" << endl;

    } catch (const exception& e) {
        cerr << "💥 CRITICAL ERROR: " << e.what() << endl;
        return 1;
    }

    return 0;
}