// Email: nitzanwa@gmail.com

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <limits>
#include <algorithm>
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

class ConsoleGUI {
private:
    unique_ptr<Game> game;
    vector<Player*> players;
    bool waitingForDecision;
    enum class DecisionType { None, Bribe, Block } currentDecisionType;
    Player* decisionPlayer;
    ActionType pendingAction;
    Player* pendingActor;

public:
    ConsoleGUI() : waitingForDecision(false), currentDecisionType(DecisionType::None), 
                   decisionPlayer(nullptr), pendingAction(ActionType::None), pendingActor(nullptr) {}

    ~ConsoleGUI() {
        resetGame();
    }

    void resetGame() {
        for (Player* p : players) {
            delete p;
        }
        players.clear();
        game.reset();
        waitingForDecision = false;
        currentDecisionType = DecisionType::None;
        decisionPlayer = nullptr;
    }

    void printHeader() {
        cout << "\n" << string(60, '=') << endl;
        cout << "🎮              COUP GAME - CONSOLE GUI              🎮" << endl;
        cout << string(60, '=') << endl;
    }

    void printGameState() {
        if (!game) return;

        cout << "\n📊 === GAME STATE === 📊" << endl;
        cout << "Bank: " << game->getBank() << " coins" << endl;
        
        try {
            if (game->isGameOver()) {
                cout << "🏁 GAME OVER! 🏁" << endl;
                try {
                    cout << "🏆 WINNER: " << game->winner() << " 🏆" << endl;
                } catch (...) {
                    cout << "Game completed." << endl;
                }
            } else {
                cout << "Current turn: " << game->turn() << endl;
            }
        } catch (...) {
            cout << "Game completed." << endl;
        }

        cout << "\n👥 PLAYERS:" << endl;
        int playerNum = 1;
        for (Player* p : players) {
            if (p && game->isAlive(*p)) {
                cout << playerNum++ << ". " << p->getName() 
                     << " (" << p->getRoleName() << ") - " 
                     << p->getCoins() << " coins";
                
                if (p->isSanctioned()) cout << " [SANCTIONED]";
                
                try {
                    if (!game->isGameOver() && game->turn() == p->getName()) {
                        cout << " ⭐ (Current Turn)";
                    }
                } catch (...) {}
                
                cout << endl;
            }
        }
        cout << string(40, '-') << endl;
    }

    void setupCallbacks() {
        for (auto* p : players) {
            // Bribe decision callback
            p->setBribeDecisionCallback([this, p](const Player& player) -> bool {
                if (&player == p) {
                    decisionPlayer = p;
                    currentDecisionType = DecisionType::Bribe;
                    waitingForDecision = true;
                    return true; // Prevent automatic turn end
                }
                return false;
            });

            // Block decision callback
            p->setBlockDecisionCallback([this, p](const Player& blocker, ActionType action, const Player* actor) -> bool {
                if (&blocker == p && actor != nullptr) {
                    bool canBlock = false;
                    string actionName = "action";
                    
                    if (p->getRoleName() == "Governor" && action == ActionType::Tax) {
                        canBlock = true;
                        actionName = "tax";
                    } else if (p->getRoleName() == "Judge" && action == ActionType::Bribe) {
                        canBlock = true;
                        actionName = "bribe";
                    } else if (p->getRoleName() == "General" && action == ActionType::Coup && p->getCoins() >= 5) {
                        canBlock = true;
                        actionName = "coup";
                    }

                    if (canBlock) {
                        decisionPlayer = p;
                        currentDecisionType = DecisionType::Block;
                        pendingAction = action;
                        pendingActor = const_cast<Player*>(actor);
                        waitingForDecision = true;
                        
                        cout << "\n🚫 BLOCKING OPPORTUNITY!" << endl;
                        cout << p->getName() << " (" << p->getRoleName() 
                             << ") can block " << actor->getName() << "'s " << actionName << "!" << endl;
                        
                        return false; // Don't block automatically
                    }
                }
                return false;
            });
        }
    }

    int getPlayerCount() {
        int count;
        cout << "\n🎯 Enter number of players (2-6): ";
        while (!(cin >> count) || count < 2 || count > 6) {
            cout << "❌ Invalid input! Enter number between 2-6: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return count;
    }

    vector<string> getPlayerNames(int count) {
        vector<string> names;
        cout << "\n📝 Enter player names:" << endl;
        
        for (int i = 0; i < count; i++) {
            string name;
            bool validName = false;
            
            while (!validName) {
                cout << "Player " << (i + 1) << " name: ";
                getline(cin, name);
                
                if (name.empty()) {
                    cout << "❌ Name cannot be empty!" << endl;
                    continue;
                }
                
                // Check for duplicates
                bool duplicate = false;
                for (const string& existingName : names) {
                    if (existingName == name) {
                        duplicate = true;
                        break;
                    }
                }
                
                if (duplicate) {
                    cout << "❌ Name already taken! Choose a different name." << endl;
                } else {
                    validName = true;
                }
            }
            
            names.push_back(name);
        }
        
        return names;
    }

    void startGame(const vector<string>& playerNames) {
        try {
            resetGame();
            game = make_unique<Game>();

            cout << "\n🎲 Creating players with random roles..." << endl;
            for (const auto& name : playerNames) {
                Player* p = PlayerFactory::randomPlayer(*game, name);
                players.push_back(p);
                cout << "✅ " << name << " is a " << p->getRoleName() << endl;
            }

            setupCallbacks();
            cout << "\n🎮 Game started! " << game->turn() << " goes first." << endl;
        } catch (const exception& e) {
            cout << "❌ Failed to start game: " << e.what() << endl;
            resetGame();
        }
    }

    Player* getCurrentPlayer() {
        if (!game || game->isGameOver()) return nullptr;

        try {
            string currentName = game->turn();
            for (auto* p : players) {
                if (p && game->isAlive(*p) && p->getName() == currentName) {
                    return p;
                }
            }
        } catch (...) {
            return nullptr;
        }
        return nullptr;
    }

    vector<Player*> getValidTargets(Player* current) {
        vector<Player*> validTargets;
        if (!current || !game) return validTargets;

        for (auto* p : players) {
            if (p && p != current && game->isAlive(*p)) {
                validTargets.push_back(p);
            }
        }
        return validTargets;
    }

    bool handleDecision() {
        if (!waitingForDecision || !decisionPlayer) return false;

        if (currentDecisionType == DecisionType::Bribe) {
            cout << "\n💰 BRIBE DECISION for " << decisionPlayer->getName() << endl;
            cout << "You can use bribe (4 coins) for a bonus action." << endl;
            cout << "Current coins: " << decisionPlayer->getCoins() << endl;
            
            if (!decisionPlayer->canUseBribe()) {
                cout << "❌ Cannot afford bribe or already used this turn." << endl;
                waitingForDecision = false;
                currentDecisionType = DecisionType::None;
                decisionPlayer->endTurn();
                return true;
            }
            
            cout << "Use bribe? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (choice == 'y' || choice == 'Y') {
                try {
                    decisionPlayer->bribe();
                    cout << "✅ " << decisionPlayer->getName() << " used bribe! Choose your bonus action." << endl;
                } catch (const exception& e) {
                    cout << "❌ Bribe failed: " << e.what() << endl;
                }
            } else {
                decisionPlayer->endTurn();
                cout << "✅ " << decisionPlayer->getName() << "'s turn ended." << endl;
            }
            
        } else if (currentDecisionType == DecisionType::Block) {
            cout << "\n🛡️ BLOCK DECISION for " << decisionPlayer->getName() << endl;
            cout << "Block the action? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            if (choice == 'y' || choice == 'Y') {
                try {
                    if (pendingAction == ActionType::Tax && decisionPlayer->getRoleName() == "Governor") {
                        static_cast<Governor*>(decisionPlayer)->undo(*pendingActor);
                    } else if (pendingAction == ActionType::Bribe && decisionPlayer->getRoleName() == "Judge") {
                        pendingActor->blockLastAction();
                    } else if (pendingAction == ActionType::Coup && decisionPlayer->getRoleName() == "General") {
                        static_cast<General*>(decisionPlayer)->blockCoup(*pendingActor);
                    }
                    cout << "✅ " << decisionPlayer->getName() << " blocked the action!" << endl;
                } catch (const exception& e) {
                    cout << "❌ Block failed: " << e.what() << endl;
                }
            } else {
                cout << "✅ " << decisionPlayer->getName() << " chose not to block." << endl;
            }
        }
        
        waitingForDecision = false;
        currentDecisionType = DecisionType::None;
        decisionPlayer = nullptr;
        pendingAction = ActionType::None;
        pendingActor = nullptr;
        return true;
    }

    void showActions(Player* current) {
        cout << "\n🎯 Available actions for " << current->getName() 
             << " (" << current->getRoleName() << "):" << endl;
        cout << "Coins: " << current->getCoins() << endl;
        
        // Check forced coup
        if (current->getCoins() >= 10) {
            cout << "⚠️ YOU MUST COUP! (10+ coins)" << endl;
            vector<Player*> targets = getValidTargets(current);
            for (size_t i = 0; i < targets.size(); i++) {
                cout << (i + 1) << ". Coup " << targets[i]->getName() << endl;
            }
            return;
        }
        
        int actionNum = 1;
        
        // Basic actions
        cout << actionNum++ << ". Gather (1 coin)" << endl;
        cout << actionNum++ << ". Tax (" << current->taxAmount() << " coins)" << endl;
        
        // Targeted actions
        vector<Player*> targets = getValidTargets(current);
        for (auto* target : targets) {
            cout << actionNum++ << ". Arrest " << target->getName() << " (steal 1 coin)" << endl;
        }
        for (auto* target : targets) {
            cout << actionNum++ << ". Sanction " << target->getName() << " (prevent economic actions)" << endl;
        }
        for (auto* target : targets) {
            cout << actionNum++ << ". Coup " << target->getName() << " (eliminate - 7 coins)" << endl;
        }
        
        // Role-specific actions
        if (dynamic_cast<Baron*>(current) && current->getCoins() >= 3) {
            cout << actionNum++ << ". Invest (3 → 6 coins)" << endl;
        }
        
        if (auto spy = dynamic_cast<Spy*>(current)) {
            for (auto* target : targets) {
                cout << actionNum++ << ". Peek at " << target->getName() << "'s coins" << endl;
            }
            for (auto* target : targets) {
                cout << actionNum++ << ". Block " << target->getName() << "'s next arrest" << endl;
            }
        }
        
        if (auto gov = dynamic_cast<Governor*>(current)) {
            for (auto* p : players) {
                if (p && p != current && game->isAlive(*p) &&
                    game->hasPendingAction() && game->getLastActor() == p &&
                    game->getLastActionType() == ActionType::Tax) {
                    cout << actionNum++ << ". Undo " << p->getName() << "'s tax" << endl;
                }
            }
        }
        
        if (current->canUseBribe()) {
            cout << actionNum++ << ". Bribe (4 coins for bonus action)" << endl;
        }
    }

    bool performAction(Player* current) {
        try {
            showActions(current);
            
            cout << "\nChoose action (number): ";
            int choice;
            if (!(cin >> choice)) {
                cout << "❌ Invalid input!" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return false;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            // Handle forced coup
            if (current->getCoins() >= 10) {
                vector<Player*> targets = getValidTargets(current);
                if (choice < 1 || choice > static_cast<int>(targets.size())) {
                    cout << "❌ Invalid choice!" << endl;
                    return false;
                }
                current->coup(*targets[choice - 1]);
                cout << "💥 " << current->getName() << " was forced to coup " << targets[choice - 1]->getName() << endl;
                return true;
            }
            
            // Regular actions
            int actionNum = 1;
            vector<Player*> targets = getValidTargets(current);
            
            // Basic actions
            if (choice == actionNum++) {
                current->gather();
                cout << "💰 " << current->getName() << " gathered 1 coin" << endl;
                return true;
            }
            if (choice == actionNum++) {
                current->tax();
                cout << "🏛️ " << current->getName() << " collected " << current->taxAmount() << " coins from tax" << endl;
                return true;
            }
            
            // Arrest actions
            for (auto* target : targets) {
                if (choice == actionNum++) {
                    current->arrest(*target);
                    cout << "👮 " << current->getName() << " arrested " << target->getName() << endl;
                    return true;
                }
            }
            
            // Sanction actions
            for (auto* target : targets) {
                if (choice == actionNum++) {
                    current->sanction(*target);
                    cout << "⛔ " << current->getName() << " sanctioned " << target->getName() << endl;
                    return true;
                }
            }
            
            // Coup actions
            for (auto* target : targets) {
                if (choice == actionNum++) {
                    current->coup(*target);
                    cout << "💥 " << current->getName() << " couped " << target->getName() << endl;
                    return true;
                }
            }
            
            // Role-specific actions
            if (auto baron = dynamic_cast<Baron*>(current)) {
                if (baron->getCoins() >= 3 && choice == actionNum++) {
                    baron->invest();
                    cout << "📈 " << current->getName() << " invested (3 → 6 coins)" << endl;
                    return true;
                }
            }
            
            if (auto spy = dynamic_cast<Spy*>(current)) {
                // Peek actions
                for (auto* target : targets) {
                    if (choice == actionNum++) {
                        int coins = spy->peekCoins(*target);
                        cout << "👁️ " << target->getName() << " has " << coins << " coins" << endl;
                        return false; // Don't end turn for peek
                    }
                }
                // Block arrest actions
                for (auto* target : targets) {
                    if (choice == actionNum++) {
                        spy->blockNextArrest(*target);
                        cout << "🚫 Blocked " << target->getName() << "'s next arrest" << endl;
                        return false; // Don't end turn for this
                    }
                }
            }
            
            if (auto gov = dynamic_cast<Governor*>(current)) {
                for (auto* p : players) {
                    if (p && p != current && game->isAlive(*p) &&
                        game->hasPendingAction() && game->getLastActor() == p &&
                        game->getLastActionType() == ActionType::Tax) {
                        if (choice == actionNum++) {
                            gov->undo(*p);
                            cout << "↩️ " << current->getName() << " undid " << p->getName() << "'s tax" << endl;
                            return false; // Don't end turn for undo
                        }
                    }
                }
            }
            
            if (current->canUseBribe() && choice == actionNum++) {
                current->bribe();
                cout << "💵 " << current->getName() << " used bribe for bonus action!" << endl;
                return false; // Allow bonus action
            }
            
            cout << "❌ Invalid choice!" << endl;
            return false;
            
        } catch (const exception& e) {
            cout << "❌ Action failed: " << e.what() << endl;
            return false;
        }
    }

    void run() {
        printHeader();
        
        cout << "\n🎮 Welcome to Coup!" << endl;
        cout << "This is a console-based version of the game." << endl;
        
        int playerCount = getPlayerCount();
        vector<string> playerNames = getPlayerNames(playerCount);
        
        startGame(playerNames);
        
        // Main game loop
        while (game && !game->isGameOver()) {
            printGameState();
            
            // Handle pending decisions first
            if (handleDecision()) {
                continue;
            }
            
            Player* current = getCurrentPlayer();
            if (!current) break;
            
            cout << "\n🎯 " << current->getName() << "'s turn!" << endl;
            
            bool actionTaken = false;
            while (!actionTaken && !game->isGameOver()) {
                actionTaken = performAction(current);
                
                // Check for decisions after action
                if (waitingForDecision) {
                    break;
                }
            }
        }
        
        // Final game state
        printGameState();
        
        cout << "\n🎉 Thanks for playing Coup! 🎉" << endl;
    }
};