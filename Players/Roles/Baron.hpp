#pragma once

#include "../../GameLogic/Game.hpp"
#include "../Player.hpp"
#include <string>

namespace coup {

    class Baron : public Player {
    public:
        Baron(Game &game, const std::string &name);

        void invest();

        std::string getRoleName() const override;
    };

}
