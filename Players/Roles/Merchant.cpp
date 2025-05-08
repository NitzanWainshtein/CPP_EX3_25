#include "Merchant.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {
    Merchant::Merchant(Game &game, const std::string &name) : Player(game, name) {
    }
    void Merchant::startTurn() {if (game.turn() != name) {
        throw std::runtime_error("Not " + name + "'s turn");
    }

        if (coins >= 10) {
            throw std::runtime_error("You must perform a coup!");
        }

        if (actionBlocked) {
            if (lastAction == ActionType::Tax) {
                BankManager::transferToBank(*this, game, taxAmount());
            }
            actionBlocked = false;
        }

        bribeUsedThisTurn = false;
        lastAction = ActionType::None;
        lastActionTarget = nullptr;
        if (coins >= 3) {
            BankManager::transferFromBank(game, *this, 1);
        }
    }

    std::string Merchant::getRoleName() const {
        return "Merchant";
    }
}
