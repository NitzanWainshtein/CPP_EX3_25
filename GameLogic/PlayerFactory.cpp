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

using namespace std;

namespace coup {
    Player *randomPlayer(Game &game, const string &name) {
        if (game.nameExists(name)) {
            throw std::runtime_error("Name '" + name + "' already exists in game");
        }
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 5);

        int num = dis(gen);
        switch (num) {
            case 0: return new Governor(game, name);
            case 1: return new Spy(game, name);
            case 2: return new Baron(game, name);
            case 3: return new General(game, name);
            case 4: return new Judge(game, name);
            case 5: return new Merchant(game, name);
            default: throw runtime_error("Invalid index for role");
        }
    }
}
