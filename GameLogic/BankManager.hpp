// Email: nitzanwa@gmail.com

#pragma once

#include "../Players/Player.hpp"
#include "Game.hpp"

namespace coup {

    /**
     * @class BankManager
     * @brief Utility class for managing coin transactions in the game.
     *
     * All methods are static and handle transfers between players and the central bank.
     */
    class BankManager {
    public:

        /**
         * @brief Transfers coins directly between two players.
         *
         * @param from The player giving coins.
         * @param to The player receiving coins.
         * @param amount Number of coins to transfer.
         * @throws std::runtime_error if 'from' has insufficient coins.
         */
        static void transferCoins(Player& from, Player& to, int amount);

        /**
         * @brief Adds coins to the central bank.
         *
         * @param game The game instance containing the bank.
         * @param amount Number of coins to add.
         * @throws std::runtime_error if amount is negative.
         */
        static void addToBank(Game& game, int amount);

        /**
         * @brief Removes coins from the central bank.
         *
         * @param game The game instance containing the bank.
         * @param amount Number of coins to remove.
         * @throws std::runtime_error if the bank has insufficient coins.
         */
        static void takeFromBank(Game& game, int amount);

        /**
         * @brief Transfers coins from a player to the bank.
         *
         * @param from The player paying coins.
         * @param game The game instance.
         * @param amount Number of coins to transfer.
         * @throws std::runtime_error if the player has insufficient coins.
         */
        static void transferToBank(Player& from, Game& game, int amount);

        /**
         * @brief Transfers coins from the bank to a player.
         *
         * @param game The game instance.
         * @param to The player receiving coins.
         * @param amount Number of coins to transfer.
         * @throws std::runtime_error if the bank has insufficient coins.
         */
        static void transferFromBank(Game& game, Player& to, int amount);
    };

}
