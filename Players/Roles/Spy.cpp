#include "Spy.hpp"
#include "../../GameLogic/Logger.hpp"

namespace coup {

    Spy::Spy(Game &game, const std::string &name)
        : Player(game, name) {}

    int Spy::peekCoins(const Player &target) const {
        int coins = target.getCoins();
        Logger::log(name + " spies on " + target.getName() +
                    " and sees " + std::to_string(coins) + " coins");
        return coins;
    }

    void Spy::blockNextArrest(Player &target) {
        target.setArrestBlocked(true);
        Logger::log(name + " blocks " + target.getName() +
                    " from using arrest in the next turn");
    }

    std::string Spy::getRoleName() const {
        return "Spy";
    }

}
