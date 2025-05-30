// Email: nitzanwa@gmail.com

#include "Player.hpp"
#include "../Players/Roles/General.hpp"
#include "../GameLogic/BankManager.hpp"
#include "../GameLogic/Logger.hpp"
#include <iostream>
#include <stdexcept>

namespace coup {

    Player::Player(Game &game, const std::string &name)
        : game(game), name(name), coins(0), sanctioned(false), arrestStatus(ArrestStatus::Available),
          lastAction(ActionType::None), lastActionTarget(nullptr),
          actionBlocked(false), arrestBlocked(false), bribeUsedThisTurn(false) {
        Logger::log("Initializing player: " + name);
        game.addPlayer(this);
    }

    std::string Player::getLastActionName() const {
        Logger::log("Fetching last action name for " + name);
        switch (lastAction) {
            case ActionType::Tax: return "Tax";
            case ActionType::Bribe: return "Bribe";
            case ActionType::Coup: return "Coup";
            case ActionType::Sanction: return "Sanction";
            case ActionType::Arrest: return "Arrest";
            case ActionType::Gather: return "Gather";
            case ActionType::Invest: return "Invest";
            default: return "None";
        }
    }

    void Player::requireTurn() const {
        Logger::log("Checking turn for " + name);
        if (game.turn() != name) {
            Logger::log("Turn check failed for " + name);
            throw std::runtime_error("Not " + name + "'s turn");
        }
        Logger::log("Turn check passed for " + name);
    }

    void Player::requireAlive(const Player &target) const {
        Logger::log("Checking if " + target.getName() + " is alive");
        if (!game.isAlive(target)) {
            Logger::log("Player " + target.getName() + " is not alive");
            throw std::runtime_error(target.getName() + " is not alive");
        }
    }

    void Player::requireNotSelf(const Player &target_player, const std::string &action) const {
        Logger::log("Checking if " + name + " is trying to " + action + " themselves");
        if (&target_player == this) {
            Logger::log("Self-action detected: cannot " + action + " yourself");
            throw std::runtime_error("Cannot " + action + " yourself");
        }
    }

    void Player::requireCanSanction(const Player &target) const {
        Logger::log("Checking if " + target.getName() + " can be sanctioned");
        if (target.isSanctioned()) {
            Logger::log("Player " + target.getName() + " is already sanctioned");
            throw std::runtime_error(target.getName() + " is already sanctioned");
        }
    }

    void Player::requireCanArrest(const Player &target) const {
        Logger::log("Checking if " + target.getName() + " can be arrested");
        if (target.getArrestStatus() == ArrestStatus::ArrestedNow) {
            Logger::log("Player " + target.getName() + " was just arrested and cannot be arrested again");
            throw std::runtime_error(target.getName() + " was just arrested and cannot be arrested again this turn");
        }
        if (target.getArrestStatus() == ArrestStatus::Cooldown) {
            Logger::log("Player " + target.getName() + " is in arrest cooldown");
            throw std::runtime_error(target.getName() + " is in arrest cooldown and cannot be arrested");
        }
        if (this->isArrestBlocked()) {
            Logger::log("Player " + name + " is blocked from arrest by Spy");
            throw std::runtime_error(name + " is blocked from arrest by Spy");
        }
        if (target.getCoins() == 0) {
            Logger::log("Player " + target.getName() + " has no coins to be arrested");
            throw std::runtime_error(target.getName() + " has no coins to arrest");
        }
    }

    void Player::startTurn() {
        Logger::log("Starting turn for " + name);
        if (coins >= 10) {
            Logger::log(name + " has 10 or more coins and must coup");
            throw std::runtime_error(name + " must perform a coup");
        }
    }

    void Player::endTurn() {
        Logger::log("Ending turn for " + name);
        if (!game.isAlive(*this)) {
            Logger::log(name + " is eliminated and cannot end turn.");
            return;
        }

        if (arrestStatus == ArrestStatus::ArrestedNow) {
            Logger::log(name + "'s arrest status changing to Cooldown");
            arrestStatus = ArrestStatus::Cooldown;
        } else if (arrestStatus == ArrestStatus::Cooldown) {
            Logger::log(name + "'s arrest status changing to Available");
            arrestStatus = ArrestStatus::Available;
        }

        // Clear turn flags
        bribeUsedThisTurn = false;
        actionBlocked = false;
        arrestBlocked = false;
        sanctioned = false; // Clear sanctions at end of turn

        game.resolvePendingAction();
        game.nextTurn();
    }

    void Player::gather() {
        Logger::log(name + " is attempting to gather coins");
        requireTurn();
        
        if (sanctioned) {
            Logger::log(name + " is sanctioned and cannot use economic actions");
            throw std::runtime_error(name + " is sanctioned and cannot gather");
        }
        
        BankManager::transferFromBank(*this, game, 1);
        Logger::log(name + " gathered 1 coin from bank");
        lastAction = ActionType::Gather;
        game.setPendingAction(this, ActionType::Gather);
    }

    void Player::tax() {
        Logger::log(name + " is collecting tax");
        requireTurn();
        
        if (sanctioned) {
            Logger::log(name + " is sanctioned and cannot use economic actions");
            throw std::runtime_error(name + " is sanctioned and cannot tax");
        }
        
        // Check for blocking BEFORE taking the money!
        game.setPendingAction(this, ActionType::Tax);
        if (game.checkForBlocking(this, ActionType::Tax)) {
            Logger::log(name + "'s tax was blocked");
            return; // Don't take the money
        }
        
        BankManager::transferFromBank(*this, game, taxAmount());
        Logger::log(name + " collected tax of " + std::to_string(taxAmount()) + " coins");
        lastAction = ActionType::Tax;
    }

    void Player::bribe() {
        Logger::log(name + " is attempting a bribe");
        requireTurn();
        if (lastAction == ActionType::None) {
            Logger::log("Cannot bribe without a previous action");
            throw std::runtime_error("Bribe can only follow a regular action");
        }
        if (lastAction == ActionType::Bribe) {
            Logger::log("Cannot bribe twice in a row");
            throw std::runtime_error("Cannot bribe twice in a row");
        }
        if (bribeUsedThisTurn) {
            Logger::log("Bribe already used this turn");
            throw std::runtime_error("Already used bribe this turn");
        }
        if (coins < 4) {
            Logger::log("Not enough coins to bribe");
            throw std::runtime_error("Need 4 coins for bribe");
        }

        BankManager::transferToBank(*this, game, 4);
        Logger::log(name + " paid 4 coins for BRIBE.");
        game.requestImmediateResponse(this, ActionType::Bribe, nullptr);

        if (actionBlocked) {
            Logger::log(name + "'s bribe was blocked");
            endTurn();
            return;
        }

        lastAction = ActionType::Bribe;
        bribeUsedThisTurn = true;
    }

    void Player::arrest(Player &target) {
        Logger::log(name + " is attempting to arrest " + target.getName());
        requireTurn();
        requireAlive(target);
        requireNotSelf(target, "arrest");
        requireCanArrest(target);

        if (coins < 1) {
            Logger::log("Not enough coins to arrest");
            throw std::runtime_error("Need at least 1 coin to arrest");
        }

        // Handle Merchant special case BEFORE transferring coins
        if (target.getRoleName() == "Merchant") {
            if (target.getCoins() < 2) {
                throw std::runtime_error("Merchant does not have 2 coins for arrest penalty");
            }
            BankManager::transferToBank(target, game, 2);
            Logger::log(target.getName() + " is a Merchant and pays 2 coins to bank instead of to attacker");
        } else {
            // Normal arrest: transfer 1 coin from target to attacker
            BankManager::transferCoins(target, *this, 1);
            Logger::log(name + " arrested " + target.getName() + " and took 1 coin");
        }

        // Handle General compensation AFTER arrest
        if (target.getRoleName() == "General") {
            BankManager::transferFromBank(target, game, 1);
            Logger::log(target.getName() + " is a General and regains 1 coin after arrest");
        }

        lastAction = ActionType::Arrest;
        lastActionTarget = &target;

        target.arrestStatus = ArrestStatus::ArrestedNow;

        game.setPendingAction(this, ActionType::Arrest, &target);
    }

    void Player::sanction(Player &target) {
        Logger::log(name + " is attempting to sanction " + target.getName());
        requireTurn();
        requireAlive(target);
        requireNotSelf(target, "sanction");
        requireCanSanction(target);

        // Calculate total cost before payment
        int totalCost = 3;
        if (target.getRoleName() == "Judge") {
            totalCost = 4; // 3 + 1 extra for Judge
        }

        if (coins < totalCost) {
            throw std::runtime_error("Need " + std::to_string(totalCost) + " coins to sanction " + target.getRoleName());
        }

        // Pay the cost
        BankManager::transferToBank(*this, game, totalCost);
        target.sanctioned = true;
        Logger::log(name + " sanctioned " + target.getName() + " (cost: " + std::to_string(totalCost) + ")");

        // Handle Baron compensation AFTER sanction
        if (target.getRoleName() == "Baron") {
            BankManager::transferFromBank(target, game, 1);
            Logger::log("Bank compensates 1 coin to Baron " + target.getName() + " due to sanction");
        }

        lastAction = ActionType::Sanction;
        lastActionTarget = &target;
        game.setPendingAction(this, ActionType::Sanction, &target);
    }

    void Player::coup(Player &target) {
        Logger::log(name + " is attempting a coup on " + target.getName());
        requireTurn();
        requireAlive(target);
        requireNotSelf(target, "coup");

        if (coins < 7) {
            Logger::log("Not enough coins to coup");
            throw std::runtime_error("Need at least 7 coins to perform a coup");
        }

        BankManager::transferToBank(*this, game, 7);
        game.eliminate(target);

        Logger::log(name + " performed a coup against " + target.getName());
        lastAction = ActionType::Coup;
        lastActionTarget = &target;
        game.setPendingAction(this, ActionType::Coup, &target);
    }

    bool Player::tryBlockAction(ActionType action, Player *actor, Player *target) {
        Logger::log(name + " is attempting to block action " + std::to_string(static_cast<int>(action)));
        return false;
    }

    void Player::blockLastAction() {
        Logger::log(name + " is blocking last action");
        actionBlocked = true;
    }

    bool Player::askForBribe() {
        Logger::log("Asking " + name + " for bribe decision");
        return bribeDecisionCallback && bribeDecisionCallback(*this);
    }

    bool Player::askForBlock(ActionType action, Player *actor, Player *target) {
        Logger::log("Asking " + name + " for block decision on action");
        return blockDecisionCallback && blockDecisionCallback(*this, action, target);
    }

}