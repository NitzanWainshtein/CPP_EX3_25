#pragma once

#include "../Player.hpp"
#include <string>

namespace coup {
    class General : public Player {
    public:
        General(Game &game, const std::string &name);

        std::string getRoleName() const override;

        void blockCoup(Player &victim);

        virtual bool shouldBlockCoup(Player& attacker, Player& target);
        bool tryBlockCoup(Player& attacker, Player& target);

        void startTurn() override;
    };
}

