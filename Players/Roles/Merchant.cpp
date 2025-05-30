#include "Merchant.hpp"
#include "../../GameLogic/BankManager.hpp"
#include "../../GameLogic/Logger.hpp"

namespace coup {

    Merchant::Merchant(Game &game, const std::string &name)
        : Player(game, name) {}

    void Merchant::startTurn() {
        Player::startTurn();

        if (coins >= 3) {
            BankManager::transferFromBank(*this, game, 1);
            Logger::log(name + " is a Merchant and receives 1 bonus coin for starting turn with 3+ coins");
        }
    }

    std::string Merchant::getRoleName() const {
        return "Merchant";
    }

}
