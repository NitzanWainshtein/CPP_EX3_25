// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"
#include <string>

namespace coup {

    /**
     * @class Spy
     * @brief Spy role with intelligence gathering and disruption abilities
     * 
     * Special abilities:
     * - Can peek at other players' coin counts
     * - Can block another player's arrest ability for one turn
     */
    class Spy : public Player {
    public:
        /**
         * @brief Constructor
         * @param game Reference to game instance
         * @param name Player's name
         */
        Spy(Game &game, const std::string &name);

        /**
         * @brief Destructor
         */
        ~Spy() override = default;
        
        /**
         * @brief Rule of Three - explicitly deleted
         */
        Spy(const Spy& other) = delete;
        Spy& operator=(const Spy& other) = delete;

        /**
         * @brief Get role name
         * @return "Spy"
         */
        std::string getRoleName() const override;

        /**
         * @brief Reveals how many coins a given player currently holds
         * @param target The player to inspect
         * @return Number of coins the target has
         */
        int peekCoins(const Player &target) const;

        /**
         * @brief Prevents a player from using the arrest action in their next turn
         * @param target The player to block
         */
        void blockNextArrest(Player &target);
    };

}