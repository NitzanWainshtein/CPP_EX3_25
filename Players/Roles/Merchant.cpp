// Email: nitzanwa@gmail.com

#include "Merchant.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

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
     * - If the player has at least 3 coins, gains 1 extra coin automatically.
     * - If the last action (e.g., Tax) was blocked, refunds the coin.
     * - Resets turn-related flags.
     * - Throws if player holds 10 or more coins (must coup).
     */
    void Merchant::startTurn() {
        if (game.turn() != name) {
            throw std::runtime_error("Not " + name + "'s turn");
        }

        if (coins >= 10) {
            throw std::runtime_error("You must perform a coup!");
        }

        // If previous action (e.g., Tax) was blocked, refund coins
        if (actionBlocked) {
            if (lastAction == ActionType::Tax) {
                BankManager::transferToBank(*this, game, taxAmount());
            }
            actionBlocked = false;
        }

        bribeUsedThisTurn = false;
        lastAction = ActionType::None;
        lastActionTarget = nullptr;

        // Special ability: +1 coin if starting with at least 3
        if (coins >= 3) {
            BankManager::transferFromBank(game, *this, 1);
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
