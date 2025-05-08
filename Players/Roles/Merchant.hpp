#pragma once

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"
#include <string>

namespace coup {
    class Merchant : public Player {
    public:
        Merchant(Game &game, const std::string &name);

        void startTurn() override;

        std::string getRoleName() const override;
    };
}
