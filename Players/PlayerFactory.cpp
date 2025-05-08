#include "PlayerFactory.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Roles/Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include <random>
#include <string>
using namespace std;
namespace coup {

    Player* randomPlayer(Game& game, const string& name) {
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 5);

        int num = dist(gen);
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
