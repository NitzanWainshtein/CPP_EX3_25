// Email: nitzanwa@gmail.com

#pragma once

#include "Game.hpp"
#include <string>

namespace coup {

    /**
     * @brief Creates a player with a randomly selected role and adds them to the game
     * 
     * @param game Reference to the current game
     * @param name Desired name for the new player
     * @return Pointer to a newly created Player with a random role
     * @throws std::runtime_error if the name already exists in the game
     */
    Player* randomPlayer(Game& game, const std::string& name);

}