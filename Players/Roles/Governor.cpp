#include "Governor.hpp"
#include "../../GameLogic/Game.hpp"
#include <stdexcept>

namespace coup {

    Governor::Governor(Game &game, const std::string &name)
        : Player(game, name) {}

    void Governor::tax() {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (sanctioned) throw std::runtime_error("Cannot tax, player is under sanctions");
        game.takeFromBank(taxAmount());
        coins += taxAmount();
        lastAction = ActionType::Tax;
    }

    int Governor::taxAmount() const {
        return 3;
    }

    void Governor::undo(Player &player) {
        if (player.getLastAction() != ActionType::Tax) {
            throw std::runtime_error("No tax action to cancel");
        }
        player.blockLastAction();
    }

    std::string Governor::roleName() const {
        return "Governor";
    }

} // namespace coup
