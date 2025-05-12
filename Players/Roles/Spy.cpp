#include "Spy.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {
    Spy::Spy(Game &game, const std::string &name) : Player(game, name) {
    }



    int Spy::peekCoins(const Player &target) const {
        return target.getCoins();
    }

    void Spy::blockNextArrest(Player &target) {
        target.blockArrestNextTurn();
    }

    std::string Spy::getRoleName() const{
        return "Spy";
    }
}
