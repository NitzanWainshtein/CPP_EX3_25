#include "General.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {
    General::General(Game &game, const std::string &name)
        : Player(game, name) {
    }

    void General::startTurn() {
        Player::startTurn();
        if (arrested) {
            coins += 1;
        }
    }

    void General::blockCoup(Player &victim) {
        if (!game.hasPendingAction()) {
            throw std::runtime_error("No action to block.");
        }
        if (game.getLastActionType() != ActionType::Coup) {
            throw std::runtime_error("Last action is not a coup.");
        }
        if (game.getLastTarget() != &victim) {
            throw std::runtime_error("This coup is not targeted at the specified victim.");
        }
        if (coins < 5) {
            throw std::runtime_error("General does not have enough coins to block the coup.");
        }

        BankManager::transferToBank(*this, game, 5);
        game.getLastActor()->blockLastAction();
    }

    bool General::shouldBlockCoup(Player &attacker, Player &target) {
        return true;
    }

    bool General::tryBlockCoup(Player &attacker, Player &target) {
        if (&target != this && !this->game.isAlive(target)) return false;
        if (coins < 5) return false;

        if (!shouldBlockCoup(attacker, target)) return false;

        BankManager::transferToBank(*this, game, 5);
        return true;
    }


    std::string General::getRoleName() const {
        return "General";
    }
}
