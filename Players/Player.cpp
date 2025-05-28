// Email: nitzanwa@gmail.com

#include "Player.hpp"
#include <iostream>
#include <ostream>
#include "../GameLogic/Game.hpp"
#include "../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    // Static member initialization
    bool Player::guiMode = false;

    // -------------------------------
    // Constructor & Destructor
    // -------------------------------

    Player::Player(Game &game, const std::string &name)
        : game(game), name(name), coins(0), sanctioned(false), arrested(false),
          lastAction(ActionType::None), lastActionTarget(nullptr), actionBlocked(false),
          arrestBlocked(false), bribeUsedThisTurn(false),
          bribeDecisionCallback(nullptr), blockDecisionCallback(nullptr) {
        game.addPlayer(this);
    }

    Player::~Player() {}

    // -------------------------------
    // Validation Utilities
    // -------------------------------

    void Player::requireTurn() const {
        if (game.turn() != name)
            throw std::runtime_error("Not your turn");
    }

    void Player::requireAlive(const Player &target) const {
        if (!game.isAlive(target))
            throw std::runtime_error("Target player is not in the game");
    }

    void Player::requireNotSelf(const Player &target_player, const std::string &action) const {
        if (target_player.getName() == this->getName())
            throw std::runtime_error("Cannot " + action + " yourself");
    }

    void Player::requireCanSanction(const Player &target) const {
        int requiredCoins = (target.getRoleName() == "Judge") ? 4 : 3;
        if (coins < requiredCoins) {
            throw std::runtime_error("Not enough coins to sanction " + target.getRoleName());
        }
    }

    void Player::requireCanArrest(const Player &target) const {
        if (arrestBlocked)
            throw std::runtime_error("You are blocked from using arrest this turn");
        if (target.arrested)
            throw std::runtime_error("Player was arrested last turn");
        if (target.getCoins() == 0 ||
            (target.getRoleName() == "Merchant" && target.getCoins() < 2))
            throw std::runtime_error("Player doesn't have enough coins to be arrested");
    }

    // -------------------------------
    // Getters & Setters
    // -------------------------------

    std::string Player::getName() const { return this->name; }
    int Player::getCoins() const { return this->coins; }
    void Player::setCoins(int coins) { this->coins = coins; }
    bool Player::isSanctioned() const { return this->sanctioned; }
    ActionType Player::getLastAction() const { return this->lastAction; }

    // -------------------------------
    // GUI Helper Methods
    // -------------------------------

    void Player::resetTurnState() {
        lastAction = ActionType::None;
        lastActionTarget = nullptr;
        bribeUsedThisTurn = false;
        actionBlocked = false;
    }

    Player::BribeDecisionCallback Player::getBribeCallback() const {
        return bribeDecisionCallback;
    }

    void Player::overrideBribeCallback(BribeDecisionCallback callback) {
        bribeDecisionCallback = callback;
    }

    bool Player::hasBribedThisTurn() const {
        return bribeUsedThisTurn;
    }

    // -------------------------------
    // Decision Callbacks
    // -------------------------------

    void Player::setBribeDecisionCallback(BribeDecisionCallback callback) {
        bribeDecisionCallback = callback;
    }

    void Player::setBlockDecisionCallback(BlockDecisionCallback callback) {
        blockDecisionCallback = callback;
    }

    bool Player::canUseBribe() const {
        return !bribeUsedThisTurn && coins >= 4;
    }

    bool Player::askForBribe() const {
        if (!canUseBribe()) return false;
        if (bribeDecisionCallback) {
            return bribeDecisionCallback(*this);
        }
        return false;
    }

    bool Player::askForBlock(ActionType action, const Player* actor) const {
        if (blockDecisionCallback) {
            if (actor == nullptr || !game.isAlive(*actor)) {
                return false;
            }
            return blockDecisionCallback(*this, action, actor);
        }
        return false;
    }

    // -------------------------------
    // State & Flags
    // -------------------------------

    void Player::blockLastAction() { this->actionBlocked = true; }
    void Player::blockArrestNextTurn() { this->arrestBlocked = true; }
    int Player::taxAmount() const { return 2; }

    // -------------------------------
    // Turn Logic
    // -------------------------------

    void Player::startTurn() {
        requireTurn();
        std::cout << name << " starts turn." << std::endl;
        if (coins >= 10) throw std::runtime_error("You must perform a coup!");
        bribeUsedThisTurn = false;
        lastAction = ActionType::None;
        lastActionTarget = nullptr;
        actionBlocked = false;
    }

    void Player::endTurn() {
        // Process pending actions BEFORE clearing state
        if (game.hasPendingAction() && game.getLastActor() == this && !actionBlocked) {
            try {
                switch (lastAction) {
                    case ActionType::Tax:
                        BankManager::transferFromBank(game, *this, taxAmount());
                        std::cout << "[DEBUG] " << name << " received " << taxAmount() << " coins from tax" << std::endl;
                        break;

                    case ActionType::Bribe:
                        // Already paid in bribe() method
                        break;

                    case ActionType::Arrest:
                        if (lastActionTarget && game.isAlive(*lastActionTarget)) {
                            if (lastActionTarget->getRoleName() == "Merchant") {
                                BankManager::transferToBank(*lastActionTarget, game, 2);
                            } else {
                                BankManager::transferCoins(*lastActionTarget, *this, 1);
                            }
                            lastActionTarget->arrested = true;
                        }
                        break;

                    case ActionType::Sanction:
                        if (lastActionTarget && game.isAlive(*lastActionTarget)) {
                            int cost = (lastActionTarget->getRoleName() == "Judge") ? 4 : 3;
                            if (coins >= cost) {
                                BankManager::transferToBank(*this, game, cost);
                                lastActionTarget->sanctioned = true;
                                if (lastActionTarget->getRoleName() == "Baron") {
                                    BankManager::transferFromBank(game, *lastActionTarget, 1);
                                }
                            }
                        }
                        break;

                    case ActionType::Coup:
                        // Already handled in coup() method
                        break;

                    default:
                        break;
                }
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to process action in endTurn: " << e.what() << std::endl;
            }
        }

        // Clear pending action
        game.resolvePendingAction();

        // Clear turn state
        sanctioned = false;
        arrested = false;
        arrestBlocked = false;
        bribeUsedThisTurn = false;
        lastAction = ActionType::None;
        lastActionTarget = nullptr;
        actionBlocked = false;

        // Advance to next turn
        game.nextTurn();
        std::cout << "[DEBUG] Turn ended for " << name << std::endl;
    }

    // -------------------------------
    // Game Actions
    // -------------------------------

    void Player::gather() {
        requireTurn();
        if (sanctioned)
            throw std::runtime_error("Cannot gather, player is under sanctions");

        BankManager::transferFromBank(game, *this, 1);
        lastAction = ActionType::Gather;
        game.resolvePendingAction();

        if (!guiMode) {
            // Console mode - check for bribe and end turn
            if (!bribeUsedThisTurn && askForBribe()) {
                return;
            }
            endTurn();
        }
        // GUI mode - don't end turn automatically
    }

    void Player::tax() {
        requireTurn();
        if (sanctioned)
            throw std::runtime_error("Cannot tax, player is under sanctions");

        game.setPendingAction(this, ActionType::Tax);
        lastAction = ActionType::Tax;
        lastActionTarget = nullptr;

        if (!guiMode) {
            // Console mode - do blocking check
            game.requestImmediateResponse(this, ActionType::Tax, nullptr);
            if (actionBlocked) {
                game.resolvePendingAction();
                if (!bribeUsedThisTurn && askForBribe()) {
                    return;
                }
                endTurn();
                return;
            }
        }
        // GUI mode - don't check blocking, just set up the action

        if (!guiMode) {
            if (!bribeUsedThisTurn && askForBribe()) {
                return;
            }
            endTurn();
        }
    }

    void Player::bribe() {
        requireTurn();
        if (lastAction == ActionType::None) {
            throw std::runtime_error("Bribe can only be used as a bonus action after a regular action");
        }
        if (bribeUsedThisTurn) {
            throw std::runtime_error("Already used bribe this turn");
        }
        if (coins < 4) {
            throw std::runtime_error("Need 4 coins for bribe");
        }

        BankManager::transferToBank(*this, game, 4);

        std::cout << "[DEBUG] " << name << " is attempting bribe..." << std::endl;

        if (!guiMode) {
            game.requestImmediateResponse(this, ActionType::Bribe, nullptr);
        }

        std::cout << "[DEBUG] requestImmediateResponse completed" << std::endl;

        if (actionBlocked) {
            std::cout << "[DEBUG] Bribe was blocked!" << std::endl;
            if (!guiMode) {
                endTurn();
            }
            return;
        }

        lastAction = ActionType::Bribe;
        bribeUsedThisTurn = true;
        // Don't end turn here - allow bonus action to be taken
    }

    void Player::arrest(Player &player) {
        requireTurn();
        requireAlive(player);
        requireNotSelf(player, "arrest");
        requireCanArrest(player);

        game.setPendingAction(this, ActionType::Arrest, &player);
        lastAction = ActionType::Arrest;
        lastActionTarget = &player;

        if (!guiMode) {
            if (!bribeUsedThisTurn && askForBribe()) {
                return;
            }
            endTurn();
        }
    }

    void Player::sanction(Player &player) {
        requireTurn();
        requireAlive(player);
        requireNotSelf(player, "sanction");
        requireCanSanction(player);

        game.setPendingAction(this, ActionType::Sanction, &player);
        lastAction = ActionType::Sanction;
        lastActionTarget = &player;

        if (!guiMode) {
            if (!bribeUsedThisTurn && askForBribe()) {
                return;
            }
            endTurn();
        }
    }

    void Player::coup(Player &player) {
        requireTurn();
        requireAlive(player);
        requireNotSelf(player, "coup");
        if (coins < 7)
            throw std::runtime_error("Not enough coins to coup");

        // Pay the cost immediately
        BankManager::transferToBank(*this, game, 7);

        if (!guiMode) {
            game.requestImmediateResponse(this, ActionType::Coup, &player);
            if (actionBlocked) {
                endTurn();
                return;
            }
        }

        // Eliminate the player if not blocked
        if (!actionBlocked) {
            game.eliminate(player);
        }

        lastAction = ActionType::Coup;
        lastActionTarget = &player;

        if (!guiMode) {
            endTurn();
        }
    }

    // -------------------------------
    // Reaction Mechanisms
    // -------------------------------

    bool Player::tryBlockAction(ActionType, Player*, Player*) {
        return false;
    }

    void Player::undo(Player & /*player*/) {
        // For specific roles only
    }

} // namespace coup