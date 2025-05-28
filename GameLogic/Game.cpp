// Email: nitzanwa@gmail.com

#include "Game.hpp"
#include "../Players/Player.hpp"
#include <stdexcept>
#include <iostream>

namespace coup {

    Game::Game()
        : current_turn_index(0), bank(200),
          lastActingPlayer(nullptr), lastActionTarget(nullptr),
          lastActionType(ActionType::None), actionPending(false) {}

    bool Game::nameExists(const std::string &name) const {
        for (const auto &player : player_list) {
            if (player != nullptr && player->getName() == name) {
                return true;
            }
        }
        return false;
    }

    void Game::addPlayer(Player *player) {
        for (const auto &existing : player_list) {
            if (existing != nullptr && existing->getName() == player->getName()) {
                throw std::runtime_error("Player name '" + player->getName() + "' already exists");
            }
        }
        player_list.push_back(player);
        std::cout << "[DEBUG] Added player: " << player->getName() << " at position " << (player_list.size() - 1) << std::endl;
    }

    std::vector<std::string> Game::players() const {
        std::vector<std::string> names;
        for (const auto &p : player_list) {
            if (p != nullptr) {
                names.push_back(p->getName());
            }
        }
        return names;
    }

    std::string Game::turn() const {
        if (player_list.empty()) throw std::runtime_error("No players in game");

        // Find first alive player starting from current index
        size_t idx = current_turn_index;
        size_t attempts = 0;

        while (attempts < player_list.size()) {
            if (player_list[idx] != nullptr) {
                std::cout << "[DEBUG] Current turn: " << player_list[idx]->getName()
                          << " (index: " << idx << ")" << std::endl;
                return player_list[idx]->getName();
            }
            idx = (idx + 1) % player_list.size();
            attempts++;
        }

        throw std::runtime_error("No alive players found");
    }

    void Game::nextTurn() {
        if (isGameOver()) return;

        std::cout << "[DEBUG] NextTurn called. Current index: " << current_turn_index << std::endl;

        // Move to next player
        size_t oldIndex = current_turn_index;
        current_turn_index = (current_turn_index + 1) % player_list.size();

        // Skip eliminated players
        size_t attempts = 0;
        while (player_list[current_turn_index] == nullptr && attempts < player_list.size()) {
            current_turn_index = (current_turn_index + 1) % player_list.size();
            attempts++;
        }

        std::cout << "[DEBUG] Turn moved from index " << oldIndex << " to " << current_turn_index << std::endl;

        if (player_list[current_turn_index] != nullptr) {
            std::cout << "[DEBUG] Next player: " << player_list[current_turn_index]->getName() << std::endl;
        }
    }

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

    bool Game::isGameOver() const {
        int count = 0;
        for (const Player *p : player_list) {
            if (p != nullptr) ++count;
        }
        return count <= 1;
    }

    std::string Game::winner() const {
        if (!isGameOver()) throw std::runtime_error("Game is not over yet");
        for (const Player *p : player_list) {
            if (p != nullptr) return p->getName();
        }
        throw std::runtime_error("No players remaining");
    }

    bool Game::isAlive(const Player &player) const {
        for (const Player *p : player_list) {
            if (p == &player) return true;
        }
        return false;
    }

    void Game::resetPlayers() {
        for (Player *p : player_list) {
            delete p;
        }
        player_list.clear();
        current_turn_index = 0;
    }

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

    void Game::requestImmediateResponse(Player *actor, ActionType action, Player *target) {
        // For GUI version, we'll make blocking optional, not automatic
        // This is just a placeholder for now
        std::cout << "[DEBUG] Blocking check for " << actor->getName()
                  << "'s action (currently disabled for GUI)" << std::endl;

        // Don't do automatic blocking in GUI version
        // The GUI should handle this through user interaction
    }

} // namespace coup