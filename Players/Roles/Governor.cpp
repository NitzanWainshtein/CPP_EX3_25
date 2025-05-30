// Email: nitzanwa@gmail.com

#include "Governor.hpp"
#include "../../GameLogic/BankManager.hpp"
#include "../../GameLogic/Logger.hpp"
#include <stdexcept>

namespace coup {

    Governor::Governor(Game &game, const std::string &name)
        : Player(game, name) {}

    std::string Governor::getRoleName() const {
        return "Governor";
    }

    void Governor::tax() {
        Logger::log(name + " (Governor) is collecting enhanced tax (3 coins)");
        requireTurn();
        BankManager::transferFromBank(*this, game, 3);
        Logger::log(name + " collected 3 coins (Governor tax)");
        lastAction = ActionType::Tax;
        game.setPendingAction(this, ActionType::Tax);
    }

    void Governor::blockTax(Player &actor) {
        Logger::log(name + " is attempting to block tax from " + actor.getName());

        if (&actor == this) {
            throw std::runtime_error("Governor cannot block their own tax");
        }

        if (!game.hasPendingAction()) {
            throw std::runtime_error("No pending action to block");
        }

        if (actor.getLastAction() != ActionType::Tax) {
            throw std::runtime_error(actor.getName() + " did not perform a tax action");
        }

        if (actor.getCoins() < actor.taxAmount()) {
            throw std::runtime_error(actor.getName() + " has insufficient coins to reverse tax");
        }

        BankManager::transferToBank(actor, game, actor.taxAmount());
        actor.blockLastAction();

        Logger::log(name + " blocked tax of " + actor.getName() +
                    ", returned " + std::to_string(actor.taxAmount()) + " coins to bank");
    }

}
