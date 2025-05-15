// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"
#include <string>

namespace coup {

    /**
     * @class Merchant
     * @brief Represents the Merchant role in the game.
     *
     * The Merchant gains 1 extra coin at the start of their turn if they have at least 3 coins.
     * If their last action was blocked (e.g., Tax), they refund the coins automatically.
     */
    class Merchant : public Player {
    public:
        /**
         * @brief Constructs a Merchant player.
         * @param game Reference to the game instance.
         * @param name The player's name.
         */
        Merchant(Game &game, const std::string &name);

        /**
         * @brief Called at the start of the turn.
         *
         * If the player has 3+ coins, they receive 1 bonus coin.
         * If a previous action was blocked (e.g., tax), coins are refunded.
         * Throws if the player has 10+ coins and must coup.
         */
        void startTurn() override;

        /**
         * @brief Returns the name of the role.
         * @return A string: "Merchant"
         */
        std::string getRoleName() const override;
    };

}
