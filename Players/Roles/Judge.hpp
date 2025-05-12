#pragma once

#include "../Player.hpp"

namespace coup {
    class Judge : public Player {
    public:
        Judge(Game& game, const std::string& name);
        std::string getRoleName() const override;

        virtual bool shouldBlockBribe(Player& attacker);
        bool tryBlockBribe(Player& attacker) override;
    };

        void undo(Player& player) override {}
    };
}
