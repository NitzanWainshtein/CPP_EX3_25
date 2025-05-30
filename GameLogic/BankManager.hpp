// Email: nitzanwa@gmail.com

#ifndef BANK_MANAGER_HPP
#define BANK_MANAGER_HPP

#include <stdexcept>
#include "Game.hpp"
#include "../Players/Player.hpp"

namespace coup {

    /**
     * @class BankManager
     * @brief Static utility class for managing coin transfers
     * 
     * Provides safe methods for transferring coins between:
     * - Bank and players
     * - Player to player
     * 
     * All methods validate sufficient funds before transfer
     */
    class BankManager {
    public:
        /**
         * @brief Transfer coins from bank to player
         * @param player Recipient player
         * @param game Game instance containing the bank
         * @param amount Number of coins to transfer
         * @throws std::runtime_error if invalid amount or insufficient bank funds
         */
        static void transferFromBank(Player &player, Game &game, int amount);
        
        /**
         * @brief Transfer coins from player to bank
         * @param player Source player
         * @param game Game instance containing the bank
         * @param amount Number of coins to transfer
         * @throws std::runtime_error if invalid amount or insufficient player funds
         */
        static void transferToBank(Player &player, Game &game, int amount);
        
        /**
         * @brief Transfer coins between players
         * @param from Source player
         * @param to Recipient player
         * @param amount Number of coins to transfer
         * @throws std::runtime_error if invalid amount or insufficient funds
         */
        static void transferCoins(Player &from, Player &to, int amount);
    };

}

#endif // BANK_MANAGER_HPP