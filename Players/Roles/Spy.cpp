#include "Spy.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    Spy::Spy(Game &game, const std::string &name) : Player(game, name) {}

    std::string Spy::getRoleName() const {
        return "Spy";
    }

    // יכולת מיוחדת: לראות כמה מטבעות יש לשחקן אחר
    int Spy::peekCoins(const Player &target) const {
        return target.getCoins();
    }

    // יכולת מיוחדת: לחסום arrest בתור הבא
    void Spy::blockNextArrest(Player &target) {
        target.blockLastAction();  // פשוט מאוד – משתמש במנגנון ה־actionBlocked
        this->lastAction = ActionType::None;
    }

}
