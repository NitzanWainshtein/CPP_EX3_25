#include "Game.hpp"
#include "../Players/Player.hpp"
#include "../Players/Roles/Judge.hpp"
#include "../Players/Roles/General.hpp"
#include <stdexcept>
#include <iostream>
#include <algorithm> // For std::find

namespace coup {
    Game::Game()
        : current_turn_index(0), bank(200), lastActingPlayer(nullptr), lastActionTarget(nullptr),
          lastActionType(ActionType::None), actionPending(false) {
    }

    bool Game::nameExists(const std::string &name) const {
        for (const auto &player: player_list) {
            if (player != nullptr && player->getName() == name) {
                return true;
            }
        }
        return false;
    }

    void Game::addPlayer(Player *player) {
        for (const auto &existing: player_list) {
            if (existing != nullptr && existing->getName() == player->getName()) {
                throw std::runtime_error("Player name '" + player->getName() + "' already exists");
            }
        }
        player_list.push_back(player);
    }

    std::vector<std::string> Game::players() const {
        std::vector<std::string> names;
        for (size_t i = 0; i < player_list.size(); i++) {
            if (player_list[i] != nullptr) {
                names.push_back(player_list[i]->getName());
            }
        }
        return names;
    }

    std::string Game::turn() const {
        if (player_list.empty()) throw std::runtime_error("No players in game");
        if (player_list[current_turn_index] == nullptr) {
            // This shouldn't happen if nextTurn works properly, but as a safeguard
            throw std::runtime_error("Current turn points to a removed player");
        }
        return player_list[current_turn_index]->getName();
    }

    void Game::nextTurn() {
        if (isGameOver()) return;

        size_t initialIndex = current_turn_index;

        do {
            current_turn_index = (current_turn_index + 1) % player_list.size();
            // If we've checked all positions and found no active player, break to avoid infinite loop
            if (current_turn_index == initialIndex) {
                std::cout << "[WARN] No other active players found in nextTurn()" << std::endl;
                break;
            }
        } while (player_list[current_turn_index] == nullptr);
    }

    void Game::eliminate(Player &player) {
        // Find the player's index in the list
        int playerIndex = -1;
        for (size_t i = 0; i < player_list.size(); i++) {
            if (player_list[i] == &player) {
                playerIndex = static_cast<int>(i);
                break;
            }
        }

        if (playerIndex == -1) {
            throw std::runtime_error("Player not found in eliminate()");
        }

        // Print info and mark player as eliminated
        std::cout << "[INFO] " << player.getName() << " has been eliminated." << std::endl;

        // Set the player to nullptr (removed from game)
        player_list[playerIndex] = nullptr;

        // If it was this player's turn, advance to next player
        if (current_turn_index == playerIndex) {
            nextTurn();
        }
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

    bool Game::isGameOver() const {
        int activePlayerCount = 0;
        for (const Player *p: player_list) {
            if (p != nullptr) activePlayerCount++;
        }
        return activePlayerCount <= 1;
    }

    bool Game::isAlive(const Player &player) const {
        for (const Player *p: player_list) {
            if (p == &player) return true;
        }
        return false;
    }

    std::string Game::winner() const {
        if (!isGameOver()) throw std::runtime_error("Game is not over yet");

        for (const Player *p: player_list) {
            if (p != nullptr) return p->getName();
        }
        throw std::runtime_error("No players remaining");
    }

    void Game::resetPlayers() {
        for (Player *p: player_list) {
            if (p != nullptr) delete p;
        }
        player_list.clear();
        current_turn_index = 0;
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
        if (!actor) return;

        for (Player *p: player_list) {
            if (p == nullptr || p == actor) continue;

            if (action == ActionType::Bribe && p->getRoleName() == "Judge") {
                Judge *judge = dynamic_cast<Judge *>(p);
                if (judge && judge->tryBlockBribe(*actor)) {
                    actor->blockLastAction();
                    return;
                }
            }

            if (action == ActionType::Coup && target != nullptr && p->getRoleName() == "General") {
                General *general = dynamic_cast<General *>(p);
                if (general && general->tryBlockCoup(*actor, *target)) {
                    actor->blockLastAction();
                    return;
                }
            }
        }
    }
} // namespace coup