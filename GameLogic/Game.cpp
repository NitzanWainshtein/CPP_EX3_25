#include "Game.hpp"
#include "Player.hpp"
#include <stdexcept>

namespace coup {

    Game::Game() : current_turn_index(0), bank(200) {}

    void Game::addPlayer(Player* player) {
        player_list.push_back(player);
    }

    std::vector<std::string> Game::players() const {
        std::vector<std::string> names;
        for (const auto& p : player_list) {
            if (p != nullptr) {
                names.push_back(p->getName());
            }
        }
        return names;
    }

    std::string Game::turn() const {
        if (player_list.empty()) throw std::runtime_error("No players in game");
        return player_list[current_turn_index]->getName();
    }

    void Game::nextTurn() {
        if (isGameOver()) return;
        do {
            current_turn_index = (current_turn_index + 1) % player_list.size();
        } while (player_list[current_turn_index] == nullptr);
    }

    void Game::eliminate(Player& player) {
        for (size_t i = 0; i < player_list.size(); ++i) {
            if (player_list[i] == &player) {
                player_list[i] = nullptr;
                return;
            }
        }
        throw std::runtime_error("Player not found");
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
        int count = 0;
        for (const Player* p : player_list) {
            if (p != nullptr) ++count;
        }
        return count == 1;
    }

    std::string Game::winner() const {
        if (!isGameOver()) throw std::runtime_error("Game is not over yet");
        for (const Player* p : player_list) {
            if (p != nullptr) return p->getName();
        }
        throw std::runtime_error("No players remaining");
    }

} // namespace coup
