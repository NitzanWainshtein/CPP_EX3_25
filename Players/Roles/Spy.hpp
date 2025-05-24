// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"
#include <string>

namespace coup {

    /**
     * @class Spy
     * @brief Represents the Spy role in the game.
     *
     * The Spy can:
     * - View how many coins another player has.
     * - Prevent another player from using the arrest action in their next turn.
     */
    class Spy : public Player {
    public:
        /**
         * @brief Constructs a Spy player.
         * @param game Reference to the game instance.
         * @param name The player's name.
         */
        Spy(Game &game, const std::string &name);

        Spy(const Spy& other) = delete;
        Spy& operator=(const Spy& other) = delete;
        Spy(Spy&& other) = delete;
        Spy& operator=(Spy&& other) = delete;

        /**
         * @brief Returns the name of the role.
         * @return A string: "Spy"
         */
        std::string getRoleName() const override;

        /**
         * @brief Reveals how many coins a given player currently holds.
         * @param target The player to inspect.
         * @return Number of coins the target has.
         */
        int peekCoins(const Player &target) const;

        /**
         * @brief Prevents a player from using the arrest action in their next turn.
         * @param target The player to block.
         */
        void blockNextArrest(Player &target);
    };

}
