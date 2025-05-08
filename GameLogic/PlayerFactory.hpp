#pragma once

#include "Game.hpp"
#include <string>

namespace coup {
    Player* randomPlayer(Game& game, const std::string& name);
}
