// Email: nitzanwa@gmail.com

#include "Merchant.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>
#include <iostream>

namespace coup {

    /**
     * @brief Constructs a Merchant player.
     * @param game Reference to the game instance.
     * @param name The name of the player.
     */
    Merchant::Merchant(Game &game, const std::string &name)
        : Player(game, name) {}

    /**
     * @brief Called at the start of the Merchant's turn.
     *
     * First calls Player::startTurn() to handle:
     * - Turn validation
     * - 10-coin coup requirement check
     * - Flag resets (bribeUsedThisTurn, actionBlocked, etc.)
     * - Debug output
     *
     * Then adds Merchant-specific ability:
     * - If player has at least 3 coins, gains 1 extra coin automatically.
     */
    void Merchant::startTurn() {
        Player::startTurn();

        if (coins >= 3) {
            BankManager::transferFromBank(game, *this, 1);
            std::cout << name << " (Merchant) receives bonus coin for having 3+ coins!" << std::endl;
        }
    }

    /**
     * @brief Returns the name of the role.
     * @return A string: "Merchant"
     */
    std::string Merchant::getRoleName() const {
        return "Merchant";
    }

}