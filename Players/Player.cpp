#include "Player.hpp"
#include "../GameLogic/Game.hpp"
#include <stdexcept>
#include "../GameLogic/BankManager.hpp"

namespace coup {

    Player::Player(Game &game, const std::string &name)
        : game(game), name(name), coins(0), sanctioned(false), arrested(false),
          lastAction(ActionType::None), lastActionTarget(nullptr), actionBlocked(false), arrestBlocked(false),
          bribeUsedThisTurn(false) {
        game.addPlayer(this);
    }

    Player::~Player() {}

    int Player::getCoins() const { return this->coins; }
    void Player::setCoins(int coins) { this->coins = coins; }
    std::string Player::getName() const { return this->name; }
    bool Player::isSanctioned() const { return this->sanctioned; }
    ActionType Player::getLastAction() const { return this->lastAction; }
    void Player::blockLastAction() { this->actionBlocked = true; }
    void Player::blockArrestNextTurn() { this->arrestBlocked = true; }
    int Player::taxAmount() const { return 2; }

    void Player::requireTurn() const {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
    }

    void Player::requireAlive(const Player &target) const {
        if (!game.isAlive(target)) throw std::runtime_error("Target player is not in the game");
    }

    void Player::startTurn() {
        requireTurn();
        if (coins >= 10) {
            throw std::runtime_error("You must perform a coup!");
        }

        bribeUsedThisTurn = false;
        lastAction = ActionType::None;
        lastActionTarget = nullptr;
        actionBlocked = false;
    }

    void Player::endTurn() {
        if (game.hasPendingAction() && game.getLastActor() == this && !actionBlocked) {
            switch (lastAction) {
                case ActionType::Tax:
                    BankManager::transferFromBank(game, *this, taxAmount());
                    break;
                case ActionType::Bribe:
                    // Nothing to finalize – cost paid earlier
                    break;
                case ActionType::Arrest:
                    if (lastActionTarget) {
                        if (lastActionTarget->getRoleName() == "Merchant") {
                            BankManager::transferToBank(*lastActionTarget, game, 2);
                        } else {
                            BankManager::transferCoins(*lastActionTarget, *this, 1);
                        }
                        lastActionTarget->arrested = true;
                    }
                    break;
                case ActionType::Sanction: {
                    int cost = (lastActionTarget && lastActionTarget->getRoleName() == "Judge") ? 4 : 3;
                    if (coins < cost) break;
                    BankManager::transferToBank(*this, game, cost);
                    if (lastActionTarget) {
                        lastActionTarget->sanctioned = true;
                        if (lastActionTarget->getRoleName() == "Baron") {
                            BankManager::transferFromBank(game, *lastActionTarget, 1);
                        }
                    }
                    break;
                }
                case ActionType::Coup:
                    if (coins < 7) break;
                    BankManager::transferToBank(*this, game, 7);
                    if (lastActionTarget) {
                        game.eliminate(*lastActionTarget);
                    }
                    break;
                default:
                    break;
            }
        }

        game.resolvePendingAction();
        sanctioned = false;
        arrested = false;
        arrestBlocked = false;
        bribeUsedThisTurn = false;
        game.nextTurn();
    }

    void Player::gather() {
        requireTurn();
        if (sanctioned) throw std::runtime_error("Cannot gather, player is under sanctions");
        BankManager::transferFromBank(game, *this, 1);
        lastAction = ActionType::Gather;
        game.resolvePendingAction();
        game.nextTurn();
    }

    void Player::tax() {
        requireTurn();
        if (sanctioned) throw std::runtime_error("Cannot tax, player is under sanctions");
        game.setPendingAction(this, ActionType::Tax);
        lastAction = ActionType::Tax;
        lastActionTarget = nullptr;
    }

    void Player::bribe() {
        requireTurn();
        if (coins < 4) throw std::runtime_error("Need 4 coins for bribe");
        BankManager::transferToBank(*this, game, 4);
        game.requestImmediateResponse(this, ActionType::Bribe, nullptr);
        if (actionBlocked) return;
        lastAction = ActionType::Bribe;
        bribeUsedThisTurn = true;
    }

    void Player::arrest(Player &player) {
        requireTurn();
        requireAlive(player);
        if (this->arrestBlocked) throw std::runtime_error("You are blocked from using arrest this turn");
        if (player.name == name) throw std::runtime_error("Cannot arrest yourself");
        if (player.arrested) throw std::runtime_error("Player was arrested last turn");
        if (player.coins == 0 || (player.getRoleName() == "Merchant" && player.coins < 2)) {
            throw std::runtime_error("Player doesn't have enough coins to be arrested");
        }

        game.setPendingAction(this, ActionType::Arrest, &player);
        lastAction = ActionType::Arrest;
        lastActionTarget = &player;
    }

    void Player::sanction(Player &player) {
        requireTurn();
        requireAlive(player);
        if (player.name == name) throw std::runtime_error("Cannot sanction yourself");
        if (player.isSanctioned()) throw std::runtime_error("Player already sanctioned");
        if (coins < 3 && player.getRoleName() != "Judge") throw std::runtime_error("Not enough coins to sanction");
        if (coins < 4 && player.getRoleName() == "Judge") throw std::runtime_error("Not enough coins to sanction Judge");

        game.setPendingAction(this, ActionType::Sanction, &player);
        lastAction = ActionType::Sanction;
        lastActionTarget = &player;
    }

    void Player::coup(Player& player) {
        requireTurn();
        requireAlive(player);
        if (player.name == name) throw std::runtime_error("Cannot coup yourself");
        if (coins < 7) throw std::runtime_error("Not enough coins to coup");

        BankManager::transferToBank(*this, game, 7);
        game.requestImmediateResponse(this, ActionType::Coup, &player);  // ← NEW

        if (actionBlocked) return;

        game.eliminate(player);
        lastAction = ActionType::Coup;
        lastActionTarget = &player;
    }


    void Player::undo(Player & /*player*/) {
        // For specific roles only
    }

}
