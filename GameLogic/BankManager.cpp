#include "BankManager.hpp"
#include <stdexcept>

namespace coup {

    void BankManager::transferCoins(Player& from, Player& to, int amount) {
        if (from.getCoins() < amount) {
            throw std::runtime_error("Not enough coins to transfer between players");
        }
        from.setCoins(from.getCoins() - amount);
        to.setCoins(to.getCoins() + amount);
    }

    void BankManager::addToBank(Game& game, int amount) {
        if (amount < 0) {
            throw std::runtime_error("Cannot add negative coins to bank");
        }
        game.addToBank(amount);
    }

    void BankManager::takeFromBank(Game& game, int amount) {
        if (amount > game.getBank()) {
            throw std::runtime_error("Not enough coins in bank");
        }
        game.takeFromBank(amount);
    }

    void BankManager::transferToBank(Player& from, Game& game, int amount) {
        if (from.getCoins() < amount) {
            throw std::runtime_error("Player does not have enough coins to transfer to bank");
        }
        from.setCoins(from.getCoins() - amount);
        game.addToBank(amount);
    }

    void BankManager::transferFromBank(Game& game, Player& to, int amount) {
        if (amount > game.getBank()) {
            throw std::runtime_error("Bank does not have enough coins");
        }
        to.setCoins(to.getCoins() + amount);
        game.takeFromBank(amount);
    }

}