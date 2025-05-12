#include "GameLogic/Game.hpp"
#include "Players/Player.hpp"
#include "Players/Roles/Governor.hpp"
#include "Players/Roles/Spy.hpp"
#include "Players/Roles/Baron.hpp"
#include "Players/Roles/Merchant.hpp"
#include "Players/Roles/General.hpp"
#include "Players/Roles/Judge.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <stdexcept>

using namespace coup;
using namespace std;

void status(const Game &game, const vector<Player *> &players) {
    cout << "\n[STATUS]" << endl;
    for (auto *p : players) {
        if (!p) continue;
        try {
            if (game.isAlive(*p)) {
                cout << " - " << p->getName() << " (" << p->getRoleName() << ") : " << p->getCoins() << " coins" << endl;
            }
        } catch (...) {
            cout << " - [DEAD PLAYER OR INVALID PTR]" << endl;
        }
    }
    cout << endl;
}

void gatherUntil(Player &p, Game &game, int targetCoins) {
    while (p.getCoins() < targetCoins && game.isAlive(p)) {
        while (game.turn() != p.getName()) game.nextTurn();
        p.gather();
    }
}

void forceTurn(Game &game, Player &p, function<void()> action) {
    while (game.turn() != p.getName()) game.nextTurn();
    try {
        action();
    } catch (const exception &e) {
        cout << "[Exception] " << e.what() << endl;
    }
    if (game.turn() == p.getName()) {
        try {
            p.endTurn();
        } catch (const exception &e) {
            cout << "[EndTurn Exception] " << e.what() << endl;
        }
    }
}

int main() {
    Game game;

    Governor governor(game, "Alice");
    Spy spy(game, "Bob");
    Baron baron(game, "Charlie");
    Merchant merchant(game, "Diana");
    General general(game, "Ethan");
    Judge judge(game, "Fiona");

    vector<Player *> players = {&governor, &spy, &baron, &merchant, &general, &judge};

    cout << "=== Starting Full Simulation ===\n" << endl;

    gatherUntil(governor, game, 3);
    gatherUntil(spy, game, 4);
    gatherUntil(baron, game, 3);
    gatherUntil(merchant, game, 3);
    gatherUntil(general, game, 7);
    gatherUntil(judge, game, 4);

    status(game, players);

    forceTurn(game, governor, [&]() { governor.tax(); });
    forceTurn(game, spy, [&]() { spy.bribe(); });
    forceTurn(game, spy, [&]() {
        if (game.isAlive(baron)) {
            cout << "[INFO] Spy peeks Baron: " << spy.peekCoins(baron) << " coins." << endl;
            spy.blockNextArrest(baron);
        }
    });
    forceTurn(game, baron, [&]() {
        if (game.isAlive(spy)) {
            try {
                baron.arrest(spy);
            } catch (const exception &e) {
                cout << "[Expected Arrest Blocked] " << e.what() << endl;
            }
        }
        baron.invest();
    });
    forceTurn(game, merchant, [&]() {
        if (game.isAlive(baron)) merchant.sanction(baron);
    });
    forceTurn(game, general, [&]() {
        if (game.isAlive(baron)) general.coup(baron);
    });

    gatherUntil(spy, game, 4);
    forceTurn(game, spy, [&]() { spy.bribe(); });
    forceTurn(game, judge, [&]() {
        if (game.isAlive(spy)) judge.tryBlockBribe(spy);
    });

    status(game, players);

    gatherUntil(governor, game, 7);
    forceTurn(game, governor, [&]() {
        if (game.isAlive(spy)) governor.coup(spy);
    });

    gatherUntil(merchant, game, 7);
    forceTurn(game, merchant, [&]() {
        if (game.isAlive(judge)) merchant.coup(judge);
    });

    gatherUntil(general, game, 7);
    forceTurn(game, general, [&]() {
        if (game.isAlive(governor)) general.coup(governor);
    });

    gatherUntil(merchant, game, 7);
    forceTurn(game, merchant, [&]() {
        if (game.isAlive(general)) merchant.coup(general);
    });

    status(game, players);

    cout << "=== Game Over ===\n";
    try {
        cout << "Winner: " << game.winner() << endl;
    } catch (const exception &e) {
        cout << "[ERROR] Could not determine winner: " << e.what() << endl;
    }

    return 0;
}
