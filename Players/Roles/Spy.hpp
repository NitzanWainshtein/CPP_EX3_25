#pragma once

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"
#include <string>

namespace coup {

    class Spy : public Player {
    public:
        Spy(Game &game, const std::string &name);

        std::string getRoleName() const override;

        // יכולת: לראות את כמות המטבעות של שחקן אחר
        int peekCoins(const Player &target) const;

        // יכולת: לחסום arrest בתור הבא של שחקן מסוים
        void blockNextArrest(Player &target);
    };

}
