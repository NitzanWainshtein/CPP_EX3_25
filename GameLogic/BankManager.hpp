#pragma once

#include "../Players/Player.hpp"
#include "Game.hpp"

namespace coup {

    class BankManager {
    public:
        static void transferCoins(Player& from, Player& to, int amount);
        static void addToBank(Game& game, int amount);
        static void takeFromBank(Game& game, int amount);
        static void transferToBank(Player& from, Game& game, int amount);
        static void transferFromBank(Game& game, Player& to, int amount);
    };

}
