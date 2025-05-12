#include "Judge.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    Judge::Judge(Game& game, const std::string& name)
        : Player(game, name) {}

    std::string Judge::getRoleName() const {
        return "Judge";
    }

    bool Judge::shouldBlockBribe(Player& attacker) {
        return true;
    }

    bool Judge::tryBlockBribe(Player& p) {
        if (p.getLastAction() == ActionType::Bribe && shouldBlockBribe(p)) {
            p.blockLastAction();
            return true;
        }
        return false;
    }

}
