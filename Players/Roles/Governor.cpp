// Email: nitzanwa@gmail.com

#include "Governor.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    /**
     * @brief Constructs a Governor player.
     * @param game Reference to the game instance.
     * @param name The player's name.
     */
    Governor::Governor(Game &game, const std::string &name)
        : Player(game, name) {}

    /**
     * @brief Performs a special version of the tax action.
     *
     * The Governor collects 3 coins instead of 2. The tax is deferred and only resolved in endTurn(),
     * allowing it to be cancelled by Governor's own undo.
     *
     * @throws std::runtime_error if the player is under sanction or it's not their turn.
     */
    void Governor::tax() {
        startTurn(); // includes requireTurn() + check for 10 coins
        if (sanctioned) throw std::runtime_error("Cannot tax, player is under sanctions");

        game.setPendingAction(this, ActionType::Tax);
        lastAction = ActionType::Tax;
        lastActionTarget = nullptr;
        // Coin transfer happens in endTurn()
    }

    /**
     * @brief Cancels a pending tax action made by the previous player.
     *
     * Only the Governor may cancel a Tax action, and only if it’s the most recent pending action.
     *
     * @param player The player who attempted the tax.
     * @throws std::runtime_error if there's no pending action or it's not a tax action.
     */
    void Governor::undo(Player &player) {
        if (!game.hasPendingAction()) {
            throw std::runtime_error("No pending action to cancel");
        }
        if (&player != game.getLastActor()) {
            throw std::runtime_error("Can only undo the last acting player's action");
        }
        if (player.getLastAction() != ActionType::Tax) {
            throw std::runtime_error("Only tax actions can be undone by Governor");
        }

        player.blockLastAction();
        game.resolvePendingAction();
    }

    /**
     * @brief Returns the special tax amount for the Governor role.
     * @return The number of coins collected when taxing (3).
     */
    int Governor::taxAmount() const {
        return 3;
    }

    /**
     * @brief Returns the name of the role.
     * @return A string: "Governor"
     */
    std::string Governor::getRoleName() const {
        return "Governor";
    }

} // namespace coup
