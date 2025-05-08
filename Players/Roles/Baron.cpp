#include "Baron.hpp"

#include <stdexcept>

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"

namespace coup {
    Baron::Baron(Game &game, const std::string &name) : Player(game, name) {
    }
    void Baron::invest() {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (coins < 3) throw std::runtime_error("Not enough coins to invest");
        coins -= 3;
        game.addToBank(3);
        coins += 6;
        game.takeFromBank(6);
        lastAction = ActionType::Invest;
    }
}
