#include "BankManager.hpp"
#include "Logger.hpp"

namespace coup {

    void BankManager::transferFromBank(Player &player, Game &game, int amount) {
        if (amount <= 0) {
            throw std::runtime_error("Invalid transfer amount from bank");
        }
        if (game.getBankCoins() < amount) {
            throw std::runtime_error("Not enough coins in the bank");
        }
        game.setBankCoins(game.getBankCoins() - amount);
        player.setCoins(player.getCoins() + amount);
        Logger::log("Bank transferred " + std::to_string(amount) + " coins to " + player.getName());
    }

    void BankManager::transferToBank(Player &player, Game &game, int amount) {
        if (amount <= 0) {
            throw std::runtime_error("Invalid transfer amount to bank");
        }
        if (player.getCoins() < amount) {
            throw std::runtime_error("Player " + player.getName() + " does not have enough coins");
        }
        player.setCoins(player.getCoins() - amount);
        game.setBankCoins(game.getBankCoins() + amount);
        Logger::log(player.getName() + " transferred " + std::to_string(amount) + " coins to bank");
    }

    void BankManager::transferCoins(Player &from, Player &to, int amount) {
        if (amount <= 0) {
            throw std::runtime_error("Invalid player-to-player transfer amount");
        }
        if (from.getCoins() < amount) {
            throw std::runtime_error("Player " + from.getName() + " does not have enough coins to transfer");
        }
        from.setCoins(from.getCoins() - amount);
        to.setCoins(to.getCoins() + amount);
        Logger::log(from.getName() + " transferred " + std::to_string(amount) + " coins to " + to.getName());
    }

}
