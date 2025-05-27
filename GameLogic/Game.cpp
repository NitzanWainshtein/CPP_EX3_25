// Email: nitzanwa@gmail.com

#include "Game.hpp"
#include "../Players/Player.hpp"
#include <stdexcept>
#include <iostream>

namespace coup {

    // -------------------------------
    // Constructor
    // -------------------------------

    /**
     * @brief Constructs a new Game instance with empty player list and initialized bank.
     */
    Game::Game()
        : current_turn_index(0), bank(200),
          lastActingPlayer(nullptr), lastActionTarget(nullptr),
          lastActionType(ActionType::None), actionPending(false) {}

    // -------------------------------
    // Player Management
    // -------------------------------

    /**
     * @brief Checks whether a player with a given name already exists in the game.
     * @param name The name to check for.
     * @return true if a player with the given name exists, false otherwise.
     */
    bool Game::nameExists(const std::string &name) const {
        for (const auto &player : player_list) {
            if (player != nullptr && player->getName() == name) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Adds a new player to the game.
     * @param player Pointer to the player object.
     * @throws std::runtime_error if a player with the same name already exists.
     */
    void Game::addPlayer(Player *player) {
        for (const auto &existing : player_list) {
            if (existing != nullptr && existing->getName() == player->getName()) {
                throw std::runtime_error("Player name '" + player->getName() + "' already exists");
            }
        }
        player_list.push_back(player);
    }

    /**
     * @brief Returns the names of all active (non-eliminated) players.
     * @return A vector of player names.
     */
    std::vector<std::string> Game::players() const {
        std::vector<std::string> names;
        for (const auto &p : player_list) {
            if (p != nullptr) {
                names.push_back(p->getName());
            }
        }
        return names;
    }

    /**
     * @brief Returns the name of the player whose turn it currently is.
     * @throws std::runtime_error if there are no players in the game.
     */
    std::string Game::turn() const {
        if (player_list.empty()) throw std::runtime_error("No players in game");

        Player* current = player_list[current_turn_index];
        if (current == nullptr) {
            throw std::runtime_error("Current player is eliminated");
        }

        return current->getName();
    }

    /**
     * @brief Advances the turn to the next alive player in circular order.
     */
    void Game::nextTurn() {
        if (isGameOver()) return;
        size_t start = current_turn_index;
        do {
            current_turn_index = (current_turn_index + 1) % player_list.size();
        } while (player_list[current_turn_index] == nullptr && current_turn_index != start);
    }

    /**
     * @brief Eliminates a player from the game.
     * @param player The player to eliminate.
     */
    void Game::eliminate(Player &player) {
        for (size_t i = 0; i < player_list.size(); ++i) {
            if (player_list[i] == &player) {
                player_list[i] = nullptr;
                std::cout << "[INFO] " << player.getName() << " has been eliminated." << std::endl;
                return;
            }
        }
        throw std::runtime_error("Player not found");
    }

    /**
     * @brief Checks whether the game is over (only one player remains).
     * @return true if the game is over, false otherwise.
     */
    bool Game::isGameOver() const {
        int count = 0;
        for (const Player *p : player_list) {
            if (p != nullptr) ++count;
        }
        return count == 1;
    }

    /**
     * @brief Returns the name of the winning player.
     * @throws std::runtime_error if the game is not over yet.
     */
    std::string Game::winner() const {
        if (!isGameOver()) throw std::runtime_error("Game is not over yet");
        for (const Player *p : player_list) {
            if (p != nullptr) return p->getName();
        }
        throw std::runtime_error("No players remaining");
    }

    /**
     * @brief Checks whether a player is still active in the game.
     * @param player The player to check.
     * @return true if the player is alive, false otherwise.
     */
    bool Game::isAlive(const Player &player) const {
        for (const Player *p : player_list) {
            if (p == &player) return true;
        }
        return false;
    }

    /**
     * @brief Resets the game state and deletes all players.
     */
    void Game::resetPlayers() {
        for (Player *p : player_list) {
            delete p;
        }
        player_list.clear();
        current_turn_index = 0;
    }

    // -------------------------------
    // Bank Management
    // -------------------------------

    int Game::getBank() const {
        return bank;
    }

    void Game::addToBank(int amount) {
        if (amount < 0) throw std::runtime_error("Cannot add negative coins to bank");
        bank += amount;
    }

    void Game::takeFromBank(int amount) {
        if (amount > bank) throw std::runtime_error("Not enough coins in the bank");
        bank -= amount;
    }

    // -------------------------------
    // Pending Actions
    // -------------------------------

    void Game::setPendingAction(Player *actor, ActionType action, Player *target) {
        lastActingPlayer = actor;
        lastActionType = action;
        lastActionTarget = target;
        actionPending = true;
    }

    void Game::resolvePendingAction() {
        lastActingPlayer = nullptr;
        lastActionType = ActionType::None;
        lastActionTarget = nullptr;
        actionPending = false;
    }

    bool Game::hasPendingAction() const {
        return actionPending;
    }

    Player *Game::getLastActor() const {
        return lastActingPlayer;
    }

    ActionType Game::getLastActionType() const {
        return lastActionType;
    }

    Player *Game::getLastTarget() const {
        return lastActionTarget;
    }

    // -------------------------------
    // Real-Time Blocking (Polymorphic)
    // -------------------------------

    /**
     * @brief Invokes potential responses from other players to block a pending action.
     *
     * This uses polymorphism — each player class can override tryBlockAction() to implement their own blocking logic.
     * @param actor The player who performed the action.
     * @param action The action that might be blocked.
     * @param target Optional target of the action.
     */
    void Game::requestImmediateResponse(Player *actor, ActionType action, Player *target) {
        // Safety check - make sure actor is valid
        if (actor == nullptr || !isAlive(*actor)) {
            std::cerr << "[ERROR] Invalid actor in requestImmediateResponse." << std::endl;
            return;
        }

        // Create a copy of player pointers to avoid issues if players are eliminated during blocking
        std::vector<Player*> playersToCheck;
        for (Player *p : player_list) {
            if (p != nullptr && p != actor && isAlive(*p)) {
                playersToCheck.push_back(p);
            }
        }

        // Now check each player
        for (Player *p : playersToCheck) {
            // Double check player is still valid
            if (!isAlive(*p)) continue;

            try {
                if (p->tryBlockAction(action, actor, target)) {
                    std::cout << "[INFO] " << p->getName() << " blocks " << actor->getName() << "'s action!" << std::endl;

                    // Make sure actor is still valid before blocking
                    if (isAlive(*actor)) {
                        actor->blockLastAction();
                    }
                    return;
                }
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Exception in tryBlockAction for player " << p->getName()
                          << ": " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "[ERROR] Unknown crash in tryBlockAction for player " << p->getName() << std::endl;
            }
        }
    }

} // namespace coup