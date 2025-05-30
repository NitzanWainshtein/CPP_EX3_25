// Email: nitzanwa@gmail.com

#include "PlayerFactory.hpp"
#include "../Players/Roles/Governor.hpp"
#include "../Players/Roles/Spy.hpp"
#include "../Players/Roles/Baron.hpp"
#include "../Players/Roles/General.hpp"
#include "../Players/Roles/Judge.hpp"
#include "../Players/Roles/Merchant.hpp"
#include "Logger.hpp"

#include <random>
#include <string>
#include <stdexcept>

namespace coup {

    Player* randomPlayer(Game &game, const std::string &name) {
        Logger::log("Attempting to add random player: " + name);

        if (game.nameExists(name)) {
            throw std::runtime_error("Name '" + name + "' already exists in game");
        }

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 5);

        int num = dis(gen);
        Player *newPlayer = nullptr;

        switch (num) {
            case 0:
                newPlayer = new Governor(game, name);
                break;
            case 1:
                newPlayer = new Spy(game, name);
                break;
            case 2:
                newPlayer = new Baron(game, name);
                break;
            case 3:
                newPlayer = new General(game, name);
                break;
            case 4:
                newPlayer = new Judge(game, name);
                break;
            case 5:
                newPlayer = new Merchant(game, name);
                break;
            default:
                throw std::runtime_error("Invalid index for role selection");
        }

        Logger::log("Added player " + name + " with role: " + newPlayer->getRoleName());
        return newPlayer;
    }

}