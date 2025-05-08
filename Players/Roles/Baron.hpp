#pragma once

#include "Players/Player.hpp"
#include "GameLogic/Game.hpp"
namespace coup {
    class Baron : public Player {
    public:
        Baron(Game &game, const std::string &name);
        void invest();

    };
}