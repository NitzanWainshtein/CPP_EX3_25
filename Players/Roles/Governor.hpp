#pragma once
#include "../Player.hpp"

namespace coup {

    class Governor : public Player {
    public:
        Governor(Game &game, const std::string &name);

        void tax() override;
        int taxAmount() const override;
        void undo(Player &player) override;
        std::string roleName() const override;
    };

}
