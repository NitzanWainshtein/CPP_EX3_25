#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../GameLogic/Game.hpp"
#include "../GameLogic/PlayerFactory.hpp"
#include "../Players/Player.hpp"
#include "../Players/Roles/Baron.hpp"

using namespace coup;

TEST_CASE("Create Baron and check properties") {
    Game game;
    Baron baron(game, "Alice");

    CHECK(baron.getName() == "Alice");
    CHECK(baron.getRoleName() == "Baron");

    // אין addCoins, אבל אם יש מטבעות דרך המשחק או פעולה — תבדוק שם
}

TEST_CASE("randomPlayer creates valid role") {
    Game game;
    Player* p = randomPlayer(game, "Bob");

    CHECK(p != nullptr);
    CHECK_FALSE(p->getName().empty());
    CHECK_FALSE(p->getRoleName().empty());

    delete p;
}

TEST_CASE("Game bank operations") {
    Game game;

    game.addToBank(5);
    CHECK(game.getBank() == 5);

    game.takeFromBank(3);
    CHECK(game.getBank() == 2);
}
