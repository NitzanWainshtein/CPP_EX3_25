// Email: nitzanwa@gmail.com

#pragma once

#include "../../GameLogic/Game.hpp"
#include "../Player.hpp"
#include <string>

namespace coup {

    /**
     * @class Merchant
     * @brief Merchant role with wealth-based bonuses and special arrest handling
     * 
     * Special abilities:
     * - Receives 1 bonus coin at start of turn if has 3+ coins
     * - When arrested, pays 2 coins to bank instead of 1 to attacker
     */
    class Merchant : public Player {
    public:
        /**
         * @brief Constructor
         * @param game Reference to game instance
         * @param name Player's name
         */
        Merchant(Game &game, const std::string &name);

        /**
         * @brief Destructor
         */
        ~Merchant() override = default;
        
        /**
         * @brief Rule of Three - explicitly deleted
         */
        Merchant(const Merchant& other) = delete;
        Merchant& operator=(const Merchant& other) = delete;

        /**
         * @brief Called at start of turn, adds bonus coin if wealthy
         */
        void startTurn() override;
        
        /**
         * @brief Get role name
         * @return "Merchant"
         */
        std::string getRoleName() const override;
    };

}