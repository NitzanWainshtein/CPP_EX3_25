// Email: nitzanwa@gmail.com

#include "BankManager.hpp"
#include <stdexcept>

namespace coup {

    /**
     * @brief Transfers coins from one player to another.
     *
     * @param from The player giving coins.
     * @param to The player receiving coins.
     * @param amount The number of coins to transfer.
     * @throws std::runtime_error if the 'from' player doesn't have enough coins.
     */
    void BankManager::transferCoins(Player& from, Player& to, int amount) {
        if (from.getCoins() < amount) {
            throw std::runtime_error("Not enough coins to transfer between players");
        }
        from.setCoins(from.getCoins() - amount);
        to.setCoins(to.getCoins() + amount);
    }

    /**
     * @brief Adds coins to the central bank.
     *
     * @param game The game instance holding the bank.
     * @param amount The number of coins to add.
     * @throws std::runtime_error if amount is negative.
     */
    void BankManager::addToBank(Game& game, int amount) {
        if (amount < 0) {
            throw std::runtime_error("Cannot add negative coins to bank");
        }
        game.addToBank(amount);
    }

    /**
     * @brief Removes coins from the central bank.
     *
     * @param game The game instance holding the bank.
     * @param amount The number of coins to remove.
     * @throws std::runtime_error if the bank does not have enough coins.
     */
    void BankManager::takeFromBank(Game& game, int amount) {
        if (amount > game.getBank()) {
            throw std::runtime_error("Not enough coins in bank");
        }
        game.takeFromBank(amount);
    }

    /**
     * @brief Transfers coins from a player to the bank.
     *
     * @param from The player paying coins.
     * @param game The game instance holding the bank.
     * @param amount The number of coins to transfer.
     * @throws std::runtime_error if the player does not have enough coins.
     */
    void BankManager::transferToBank(Player& from, Game& game, int amount) {
        if (from.getCoins() < amount) {
            throw std::runtime_error("Player does not have enough coins to transfer to bank");
        }
        from.setCoins(from.getCoins() - amount);
        game.addToBank(amount);
    }

    /**
     * @brief Transfers coins from the bank to a player.
     *
     * @param game The game instance holding the bank.
     * @param to The player receiving coins.
     * @param amount The number of coins to transfer.
     * @throws std::runtime_error if the bank does not have enough coins.
     */
    void BankManager::transferFromBank(Game& game, Player& to, int amount) {
        if (amount > game.getBank()) {
            throw std::runtime_error("Bank does not have enough coins");
        }
        to.setCoins(to.getCoins() + amount);
        game.takeFromBank(amount);
    }

}