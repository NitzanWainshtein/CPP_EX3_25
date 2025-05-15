// Email: nitzanwa@gmail.com

#include "Judge.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    /**
     * @brief Constructs a Judge player.
     * @param game Reference to the current game.
     * @param name The name of the player.
     */
    Judge::Judge(Game& game, const std::string& name)
        : Player(game, name) {}

    /**
     * @brief Returns the name of the role.
     * @return A string: "Judge"
     */
    std::string Judge::getRoleName() const {
        return "Judge";
    }

    /**
     * @brief Determines whether the Judge wants to block a bribe action.
     *
     * Currently always returns true.
     * @param actingPlayer The player who performed the bribe.
     * @return true if the bribe should be blocked.
     */
    bool Judge::shouldBlockBribe(Player& actingPlayer) {
        return true;
    }

    /**
     * @brief Attempts to block a bribe action from the given player.
     *
     * If the player's last action was a bribe and shouldBlockBribe returns true,
     * the bribe is blocked.
     *
     * @param player The player who performed the bribe.
     * @return true if the bribe was blocked.
     */
    bool Judge::tryBlockBribe(Player& player) {
        if (player.getLastAction() == ActionType::Bribe && shouldBlockBribe(player)) {
            player.blockLastAction();
            return true;
        }
        return false;
    }

    /**
     * @brief General override for action blocking.
     *
     * Called by the game engine to allow Judge to react to a bribe.
     *
     * @param action The type of action being attempted.
     * @param actor The player who performed the action.
     * @param target (unused) Optional target player.
     * @return true if the action was blocked.
     */
    bool Judge::tryBlockAction(ActionType action, Player* actor, Player*) {
        if (action == ActionType::Bribe) {
            return tryBlockBribe(*actor);
        }
        return false;
    }

}
