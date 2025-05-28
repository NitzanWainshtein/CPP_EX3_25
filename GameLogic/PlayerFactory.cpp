// Email: nitzanwa@gmail.com

#include "PlayerFactory.hpp"
#include "../Players/Roles/Governor.hpp"
#include "../Players/Roles/Spy.hpp"
#include "../Players/Roles/Baron.hpp"
#include "../Players/Roles/General.hpp"
#include "../Players/Roles/Judge.hpp"
#include "../Players/Roles/Merchant.hpp"

#include <random>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;

namespace coup {

    Player *randomPlayer(Game &game, const string &name) {
        // Validate name
        if (name.empty()) {
            throw std::runtime_error("Player name cannot be empty");
        }

        if (game.nameExists(name)) {
            throw std::runtime_error("Name '" + name + "' already exists in game");
        }

        // Create random number generator
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 5);

        Player* newPlayer = nullptr;

        try {
            int num = dis(gen);
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
                    throw runtime_error("Invalid random number for role selection");
            }

            std::cout << "[DEBUG] Created player: " << name << " as " << newPlayer->getRoleName() << std::endl;
            return newPlayer;

        } catch (const std::exception& e) {
            // Clean up if allocation succeeded but constructor failed
            delete newPlayer;
            throw std::runtime_error("Failed to create player: " + std::string(e.what()));
        }
    }

}