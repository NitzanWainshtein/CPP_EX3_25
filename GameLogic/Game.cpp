// Email: nitzanwa@gmail.com

#include "Game.hpp"
#include "Logger.hpp"
#include "../Players/Player.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace coup {

    Game::Game()
        : current_turn_index(0), bankCoins(200), isConsoleMode(true),
          pendingActionActor(nullptr), pendingActionType(ActionType::None), pendingActionTarget(nullptr),
          lastWinnerName("") {
        Logger::log("New game initialized with 200 coins in the bank.");
    }

    Game::~Game() {
        Logger::log("Game destructor called - cleaning up");
        player_list.clear();
        Logger::log("Game cleanup completed");
    }

    void Game::addPlayer(Player *player) {
        if (!lastWinnerName.empty()) {
            throw std::runtime_error("Cannot add players after game has ended. Please reset the game.");
        }
        if (player_list.size() >= 6) {
            throw std::runtime_error("Cannot add more than 6 players");
        }
        for (auto *p : player_list) {
            if (p && p->getName() == player->getName()) {
                throw std::runtime_error("Player name already exists: " + player->getName());
            }
        }
        player_list.push_back(player);
        Logger::log("Player '" + player->getName() + "' added to the game. Total players: " + std::to_string(player_list.size()));

        if (player_list.size() < 2) {
            Logger::log("Warning: less than 2 players — game cannot start.");
        }
    }

    /**
     * @brief Gets the name of the current player whose turn it is
     * @return Current player's name
     * @throws std::runtime_error if no players exist or no alive players found
     * 
     * FIXED: This function now properly handles eliminated players (nullptr entries)
     * by searching for the next alive player and updating current_turn_index accordingly.
     * This prevents segmentation faults when accessing eliminated players.
     */
    std::string Game::turn() const {
        if (player_list.empty()) {
            throw std::runtime_error("No players in the game");
        }
        
        // Find the next alive player starting from current_turn_index
        for (size_t i = 0; i < player_list.size(); ++i) {
            size_t index = (current_turn_index + i) % player_list.size();
            if (player_list[index] != nullptr) {
                // Update current_turn_index to point to alive player
                // Safe const_cast since we're maintaining logical const-ness
                const_cast<Game*>(this)->current_turn_index = index;
                return player_list[index]->getName();
            }
        }
        
        throw std::runtime_error("No alive players found");
    }

    /**
     * @brief Advances to the next player's turn, skipping eliminated players
     * 
     * FIXED: Added safety check to prevent infinite loops when searching for
     * the next alive player. The function now counts attempts and breaks if
     * it searches through all players without finding an alive one.
     */
    void Game::nextTurn() {
        if (isGameOver()) {
            Logger::log("Game over — no more turns.");
            return;
        }

        // Find next alive player with safety counter
        size_t attempts = 0;
        size_t original_index = current_turn_index;
        
        do {
            current_turn_index = (current_turn_index + 1) % player_list.size();
            attempts++;
            
            // Prevent infinite loop - if we've checked all positions, break
            if (attempts >= player_list.size()) {
                Logger::log("Error: Could not find next alive player after checking all positions");
                // Reset to original position and break
                current_turn_index = original_index;
                break;
            }
        } while (player_list[current_turn_index] == nullptr);

        // Verify we found a valid player
        if (player_list[current_turn_index] != nullptr) {
            Logger::log("Next turn: " + player_list[current_turn_index]->getName());
        } else {
            Logger::log("Warning: Could not advance to next alive player");
        }
    }

    std::vector<std::string> Game::players() const {
        std::vector<std::string> active_players;
        for (auto *p : player_list) {
            if (p != nullptr) {
                active_players.push_back(p->getName());
            }
        }
        std::ostringstream oss;
        for (const auto &n : active_players) {
            oss << n << " ";
        }
        Logger::log("Active players: " + oss.str());
        return active_players;
    }

    void Game::setBankCoins(int coins) {
        if (coins < 0) {
            throw std::runtime_error("Bank cannot hold negative coins");
        }
        bankCoins = coins;
        Logger::log("Bank coins set to " + std::to_string(bankCoins));
    }

    int Game::getBankCoins() const {
        return bankCoins;
    }

    bool Game::isAlive(const Player &player) const {
        for (auto *p : player_list) {
            if (p == &player) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Eliminates a player from the game by setting their slot to nullptr
     * @param player Reference to the player to eliminate
     * 
     * ENHANCED: Added comprehensive logging and improved winner detection logic.
     * The function now properly handles the case where eliminating a player
     * results in an immediate game over condition.
     */
    void Game::eliminate(Player &player) {
        Logger::log("Attempting to eliminate player: " + player.getName());
        
        bool playerFound = false;
        for (size_t i = 0; i < player_list.size(); ++i) {
            if (player_list[i] == &player) {
                player_list[i] = nullptr;
                playerFound = true;
                Logger::log("Player '" + player.getName() + "' has been eliminated from position " + std::to_string(i));
                break;
            }
        }
        
        if (!playerFound) {
            Logger::log("Warning: Player '" + player.getName() + "' was not found in the game for elimination");
            return;
        }

        // Check for immediate game over after elimination
        if (isGameOver()) {
            // Find and cache the winner immediately
            for (auto *p : player_list) {
                if (p != nullptr) {
                    lastWinnerName = p->getName();
                    Logger::log("Game over detected! Winner declared: " + lastWinnerName);
                    break;
                }
            }
            
            if (lastWinnerName.empty()) {
                Logger::log("Warning: Game over detected but no winner found!");
            }
        }
    }

    bool Game::isGameOver() const {
        int alive_count = 0;
        for (auto *p : player_list) {
            if (p != nullptr) {
                ++alive_count;
            }
        }
        return alive_count <= 1; // Changed to <= 1 for safety (handles 0 or 1 players)
    }

    /**
     * @brief Gets the winner's name when game is over
     * @return Name of the winning player
     * @throws std::runtime_error if game is not over or no winner found
     * 
     * ENHANCED: Improved error handling and winner detection with fallback logic.
     */
    std::string Game::winner() const {
        // Return cached winner if available
        if (!lastWinnerName.empty()) {
            Logger::log("Returning cached winner: " + lastWinnerName);
            return lastWinnerName;
        }
        
        if (!isGameOver()) {
            throw std::runtime_error("Game is not over yet");
        }
        
        // Search for the remaining alive player
        for (auto *p : player_list) {
            if (p != nullptr) {
                Logger::log("Found winner: " + p->getName());
                return p->getName();
            }
        }
        
        throw std::runtime_error("No players left, no winner found");
    }

    void Game::resetGame() {
        Logger::log("Resetting game...");
        player_list.clear();
        current_turn_index = 0;
        bankCoins = 200;
        pendingActionActor = nullptr;
        pendingActionType = ActionType::None;
        pendingActionTarget = nullptr;
        lastWinnerName.clear();
        isConsoleMode = true;
    }

    void Game::setPendingAction(Player *actor, ActionType actionType, Player *target) {
        pendingActionActor = actor;
        pendingActionType = actionType;
        pendingActionTarget = target;
        Logger::log("Pending action set: " + actor->getName() + " -> " + std::to_string(static_cast<int>(actionType)));
    }

    bool Game::hasPendingAction() const {
        return pendingActionActor != nullptr;
    }

    Player *Game::getLastActor() const {
        return pendingActionActor;
    }

    void Game::resolvePendingAction() {
        if (pendingActionActor) {
            Logger::log("Resolving pending action for " + pendingActionActor->getName());
        }
        pendingActionActor = nullptr;
        pendingActionType = ActionType::None;
        pendingActionTarget = nullptr;
    }

    void Game::requestImmediateResponse(Player *actor, ActionType action, Player *target) {
        Logger::log("Requesting immediate response for action: " + std::to_string(static_cast<int>(action)) +
                    " by " + actor->getName());
        checkForBlocking(actor, action, target);
    }

    bool Game::nameExists(const std::string &name) const {
        for (const auto *p : player_list) {
            if (p && p->getName() == name) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Gets all players still alive in the game
     * @return Vector of pointers to alive players (non-nullptr entries)
     * 
     * This function is safe to call after eliminations and will only return
     * valid player pointers.
     */
    std::vector<Player*> Game::getAllAlivePlayers() const {
        std::vector<Player*> alive;
        for (auto *p : player_list) {
            if (p != nullptr) {
                alive.push_back(p);
            }
        }
        return alive;
    }

    /**
     * @brief Gets the current player whose turn it is
     * @return Pointer to current player or nullptr if no alive players
     * 
     * FIXED: This function now safely handles eliminated players by searching
     * for the next alive player starting from current_turn_index. It will never
     * return a nullptr from an eliminated player position.
     */
    Player* Game::getCurrentPlayer() const {
        if (player_list.empty()) {
            Logger::log("getCurrentPlayer: No players in game");
            return nullptr;
        }
        
        // Find alive player starting from current index
        for (size_t i = 0; i < player_list.size(); ++i) {
            size_t index = (current_turn_index + i) % player_list.size();
            if (player_list[index] != nullptr) {
                // Update current_turn_index for consistency
                const_cast<Game*>(this)->current_turn_index = index;
                return player_list[index];
            }
        }
        
        Logger::log("getCurrentPlayer: No alive players found");
        return nullptr;
    }

    std::string Game::getActionName(ActionType action) const {
        switch (action) {
            case ActionType::Gather: return "Gather";
            case ActionType::Tax: return "Tax";
            case ActionType::Bribe: return "Bribe";
            case ActionType::Arrest: return "Arrest";
            case ActionType::Sanction: return "Sanction";
            case ActionType::Coup: return "Coup";
            case ActionType::Invest: return "Invest";
            default: return "None";
        }
    }

    /**
     * @brief Checks if any player can block an action and executes the block if chosen
     * @param actor Player performing the action
     * @param action Type of action being performed
     * @param target Target player (optional)
     * @return true if action was blocked, false otherwise
     * 
     * ENHANCED: Added safety checks for nullptr players during iteration.
     */
    bool Game::checkForBlocking(Player* actor, ActionType action, Player* target) {
        Logger::log("Checking if anyone wants to block " + actor->getName() + "'s " + getActionName(action));
        
        auto alivePlayers = getAllAlivePlayers();
        for (Player* p : alivePlayers) {
            // Skip if same player or nullptr (extra safety)
            if (p == actor || p == nullptr) continue;
            
            if (canPlayerBlock(p, action, actor, target)) {
                if (isConsoleMode) {
                    std::cout << "\n⚠️  BLOCKING OPPORTUNITY ⚠️" << std::endl;
                    std::cout << p->getName() << " (" << p->getRoleName() 
                             << "), do you want to block " << actor->getName() 
                             << "'s " << getActionName(action) << "? (y/n): ";
                    char choice;
                    std::cin >> choice;
                    if (choice == 'y' || choice == 'Y') {
                        executeBlock(p, action, actor, target);
                        return true;
                    }
                } else {
                    if (p->askForBlock(action, actor, target)) {
                        executeBlock(p, action, actor, target);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool Game::canPlayerBlock(Player* blocker, ActionType action, Player* actor, Player* target) const {
        // Additional safety check
        if (!blocker || !actor) return false;
        
        if (blocker->getRoleName() == "Governor" && action == ActionType::Tax && blocker != actor) {
            return true;
        }
        if (blocker->getRoleName() == "Judge" && action == ActionType::Bribe && blocker != actor) {
            return true;
        }
        if (blocker->getRoleName() == "General" && action == ActionType::Coup && blocker->getCoins() >= 5) {
            return true;
        }
        return false;
    }

    /**
     * @brief Executes a block action by a player
     * @param blocker Player performing the block
     * @param action Type of action being blocked
     * @param actor Original player who performed the action
     * @param target Original target of the action
     */
    void Game::executeBlock(Player* blocker, ActionType action, Player* actor, Player* target) {
        // Safety checks
        if (!blocker || !actor) {
            Logger::log("Error: Invalid players in executeBlock");
            return;
        }
        
        Logger::log(blocker->getName() + " is blocking " + actor->getName() + "'s " + getActionName(action));
        
        if (blocker->getRoleName() == "Governor" && action == ActionType::Tax) {
            if (actor->getCoins() >= actor->taxAmount()) {
                actor->setCoins(actor->getCoins() - actor->taxAmount());
                setBankCoins(getBankCoins() + actor->taxAmount());
                Logger::log("Governor blocked tax - " + std::to_string(actor->taxAmount()) + " coins returned to bank");
            }
        }
        else if (blocker->getRoleName() == "Judge" && action == ActionType::Bribe) {
            actor->blockLastAction();
            Logger::log("Judge blocked bribe - " + actor->getName() + " loses 4 coins permanently");
        }
        else if (blocker->getRoleName() == "General" && action == ActionType::Coup) {
            blocker->setCoins(blocker->getCoins() - 5);
            setBankCoins(getBankCoins() + 5);
            
            // Fixed: Removed problematic loop that served no purpose
            Logger::log("General blocked coup - paid 5 coins, " + (target ? target->getName() : "target") + " is safe");
        }
        
        actor->blockLastAction();
    }

}