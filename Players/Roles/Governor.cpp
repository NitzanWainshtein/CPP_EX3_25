#include "Governor.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    Governor::Governor(Game &game, const std::string &name)
        : Player(game, name) {}

    void Governor::tax() {
        startTurn();
        if (sanctioned) throw std::runtime_error("Cannot tax, player is under sanctions");

        BankManager::transferFromBank(game, *this, taxAmount());
        lastAction = ActionType::Tax;

        endTurn();
    }

    void Governor::undo(Player &player) {
        if (player.getLastAction() != ActionType::Tax) {
            throw std::runtime_error("No tax action to cancel");
        }
        player.blockLastAction();
    }

    int Governor::taxAmount() const {
        return 3;
    }

    std::string Governor::getRoleName() const {
        return "Governor";
    }

}
