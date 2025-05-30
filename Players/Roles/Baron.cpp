#include "Baron.hpp"
#include "../../GameLogic/BankManager.hpp"
#include "../../GameLogic/Logger.hpp"
#include <stdexcept>

namespace coup {

    Baron::Baron(Game &game, const std::string &name)
        : Player(game, name) {}

    void Baron::invest() {
        Logger::log(name + " is attempting to invest");

        if (game.turn() != name) {
            Logger::log("Invest failed: not " + name + "'s turn");
            throw std::runtime_error("Not your turn");
        }
        if (coins < 3) {
            Logger::log("Invest failed: not enough coins");
            throw std::runtime_error("Not enough coins to invest");
        }

        BankManager::transferToBank(*this, game, 3);
        BankManager::transferFromBank(*this, game, 6);

        Logger::log(name + " invested 3 coins and received 6 coins");

        lastAction = ActionType::Invest;

        if (!bribeUsedThisTurn && askForBribe()) {
            return;
        }
    }

    std::string Baron::getRoleName() const {
        return "Baron";
    }

}
