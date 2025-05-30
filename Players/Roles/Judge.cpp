// Email: nitzanwa@gmail.com

#include "Judge.hpp"
#include "../../GameLogic/BankManager.hpp"
#include "../../GameLogic/Logger.hpp"
#include <stdexcept>

namespace coup {

    Judge::Judge(Game &game, const std::string &name)
        : Player(game, name) {}

    std::string Judge::getRoleName() const {
        return "Judge";
    }

    void Judge::blockBribe(Player &actor) {
        Logger::log(name + " is attempting to block bribe from " + actor.getName());

        if (&actor == this) {
            throw std::runtime_error("Judge cannot block their own bribe");
        }

        if (!game.hasPendingAction()) {
            throw std::runtime_error("No pending action to block");
        }

        if (actor.getLastAction() != ActionType::Bribe) {
            throw std::runtime_error(actor.getName() + " did not perform a bribe action");
        }

        if (actor.getCoins() < 0) {
            throw std::runtime_error(actor.getName() + " has invalid coin state");
        }

        // Important: money was already paid to bank during bribe
        actor.blockLastAction();
        Logger::log(name + " blocked bribe of " + actor.getName() + ", they lose their 4 coins permanently");
    }

}
