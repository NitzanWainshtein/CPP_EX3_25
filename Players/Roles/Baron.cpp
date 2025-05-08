#include "Baron.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    Baron::Baron(Game &game, const std::string &name) : Player(game, name) {}

    void Baron::invest() {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (coins < 3) throw std::runtime_error("Not enough coins to invest");

        BankManager::transferToBank(*this, game, 3);
        BankManager::transferFromBank(game, *this, 6);

        lastAction = ActionType::Invest;
    }

    std::string Baron::getRoleName() const {
        return "Baron";
    }

}
