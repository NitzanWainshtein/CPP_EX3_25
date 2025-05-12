#include "Governor.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    Governor::Governor(Game &game, const std::string &name)
        : Player(game, name) {}

    void Governor::tax() {
        startTurn();
        if (sanctioned) throw std::runtime_error("Cannot tax, player is under sanctions");
        game.setPendingAction(this, ActionType::Tax);
        lastAction = ActionType::Tax;
        lastActionTarget = nullptr;
        endTurn(); // Added endTurn call to finish action
    }

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

    int Governor::taxAmount() const {
        return 3;
    }

    std::string Governor::getRoleName() const {
        return "Governor";
    }

} // namespace coup