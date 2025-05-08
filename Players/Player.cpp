#include "Player.hpp"
#include "Game.hpp"
#include <stdexcept>

namespace coup {

    Player::Player(Game &game, const std::string &name)
        : game(game), name(name), coins(0), sanctioned(false), arrested(false),
          lastAction(ActionType::None), lastActionTarget(nullptr), actionBlocked(false), bribeUsedThisTurn(false) {
        game.addPlayer(this);
    }

    Player::~Player() {}

    int Player::getCoins() const { return coins; }
    void Player::setCoins(int coins) { this->coins = coins; }
    std::string Player::getName() const { return name; }
    bool Player::isSanctioned() const { return sanctioned; }
    ActionType Player::getLastAction() const { return lastAction; }
    void Player::blockLastAction() { actionBlocked = true; }

    int Player::taxAmount() const { return 2; }

    void Player::startTurn() {
        if (game.turn() != name) {
            throw std::runtime_error("Not " + name + "'s turn");
        }

        if (coins >= 10) {
            throw std::runtime_error("You must perform a coup!");
        }

        if (actionBlocked) {
            if (lastAction == ActionType::Tax) {
                coins -= taxAmount();
                game.addToBank(taxAmount());
            }
            actionBlocked = false;
        }

        bribeUsedThisTurn = false;
        lastAction = ActionType::None;
        lastActionTarget = nullptr;
    }

    void Player::endTurn() {
        if (bribeUsedThisTurn) {
            bribeUsedThisTurn = false;
            return;
        }

        sanctioned = false;
        arrested = false;
        game.nextTurn();
    }

    void Player::gather() {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (sanctioned) throw std::runtime_error("Cannot gather, player is under sanctions");
        game.takeFromBank(1);
        coins++;
        lastAction = ActionType::Gather;
    }

    void Player::tax() {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (sanctioned) throw std::runtime_error("Cannot tax, player is under sanctions");
        game.takeFromBank(taxAmount());
        coins += taxAmount();
        lastAction = ActionType::Tax;
    }

    void Player::bribe() {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (coins < 4) throw std::runtime_error("Must have at least 4 coins to bribe");
        coins -= 4;
        game.addToBank(4);
        lastAction = ActionType::Bribe;
        bribeUsedThisTurn = true;
    }

    void Player::arrest(Player &player) {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (player.name == name) throw std::runtime_error("Cannot arrest yourself");
        if (player.arrested) throw std::runtime_error("Player was arrested last turn");
        if (player.coins == 0) throw std::runtime_error("Player has no coins");
        player.coins--;
        coins++;
        player.arrested = true;
        lastAction = ActionType::Arrest;
        lastActionTarget = &player;
    }

    void Player::sanction(Player &player) {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (player.name == name) throw std::runtime_error("Cannot sanction yourself");
        if (player.isSanctioned()) throw std::runtime_error("Player already sanctioned");
        if (coins < 3) throw std::runtime_error("Not enough coins to sanction");
        coins -= 3;
        game.addToBank(3);
        player.sanctioned = true;
        if (player.getRoleName() == "Baron") {
            player.coins+=1;
            game.takeFromBank(1);
        }
        lastAction = ActionType::Sanction;
        lastActionTarget = &player;
    }

    void Player::coup(Player &player) {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (player.name == name) throw std::runtime_error("Cannot coup yourself");
        if (coins < 7) throw std::runtime_error("Not enough coins to coup");
        coins -= 7;
        game.addToBank(7);
        game.eliminate(player);
        lastAction = ActionType::Coup;
        lastActionTarget = &player;
    }

    void Player::undo(Player & /*player*/) {
        // to be implemented by specific roles
    }

} // namespace coup
