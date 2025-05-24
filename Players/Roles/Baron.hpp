// Email: nitzanwa@gmail.com

#pragma once

#include "../../GameLogic/Game.hpp"
#include "../Player.hpp"
#include <string>

namespace coup {

    /**
     * @class Baron
     * @brief Represents the Baron role in the game.
     *
     * The Baron can perform an investment action, paying 3 coins to receive 6 in return.
     * Additionally, if targeted by a sanction, the Baron receives 1 coin as compensation (handled in Player logic).
     */
    class Baron : public Player {
    public:
        /**
         * @brief Constructs a Baron player.
         * @param game Reference to the game instance.
         * @param name The player's name.
         */
        Baron(Game &game, const std::string &name);

        Baron(const Baron& other) = delete;
        Baron& operator=(const Baron& other) = delete;
        Baron(Baron&& other) = delete;
        Baron& operator=(Baron&& other) = delete;

        /**
         * @brief Special ability: Invest 3 coins to receive 6.
         * @throws std::runtime_error if not the player's turn or insufficient coins.
         */
        void invest();

        /**
         * @brief Returns the name of the role.
         * @return A string: "Baron".
         */
        std::string getRoleName() const override;
    };

}
