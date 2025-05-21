// Email: nitzanwa@gmail.com

#include "Player.hpp"

#include <iostream>
#include <ostream>

#include "../GameLogic/Game.hpp"
#include "../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    // -------------------------------
    // Constructor & Destructor
    // -------------------------------

    /**
     * @brief Constructs a new Player and adds it to the game.
     * @param game Reference to the game instance.
     * @param name Name of the player.
     */
    Player::Player(Game &game, const std::string &name)
        : game(game), name(name), coins(0), sanctioned(false), arrested(false),
          lastAction(ActionType::None), lastActionTarget(nullptr), actionBlocked(false),
          arrestBlocked(false), bribeUsedThisTurn(false),
          bribeDecisionCallback(nullptr), blockDecisionCallback(nullptr) {
        game.addPlayer(this);
    }

    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    Player::~Player() {}

    // -------------------------------
    // Validation Utilities
    // -------------------------------

    /**
     * @brief Verifies that it's currently this player's turn.
     * @throws std::runtime_error if it's not the player's turn.
     */
    void Player::requireTurn() const {
        if (game.turn() != name)
            throw std::runtime_error("Not your turn");
    }

    /**
     * @brief Verifies that the target player is still alive in the game.
     * @param target The player to check.
     * @throws std::runtime_error if the target is not alive.
     */
    void Player::requireAlive(const Player &target) const {
        if (!game.isAlive(target))
            throw std::runtime_error("Target player is not in the game");
    }

    /**
     * @brief Verifies that the target player is not this player.
     * @param target_player The player to compare with.
     * @param action Action name for the error message.
     * @throws std::runtime_error if trying to act on self.
     */
    void Player::requireNotSelf(const Player &target_player, const std::string &action) const {
        if (target_player.getName() == this->getName())
            throw std::runtime_error("Cannot " + action + " yourself");
    }

    /**
     * @brief Validates whether the player can perform sanction on the target.
     * @param target The target player.
     * @throws std::runtime_error if player already sanctioned or insufficient coins.
     */
    void Player::requireCanSanction(const Player &target) const {
        if (target.isSanctioned())
            throw std::runtime_error("Player is already sanctioned");
        if (coins < 3 && target.getRoleName() != "Judge")
            throw std::runtime_error("Not enough coins to sanction player");
        if (coins < 4 && target.getRoleName() == "Judge")
            throw std::runtime_error("Not enough coins to sanction Judge");
    }

    /**
     * @brief Validates whether the player can arrest the target.
     * @param target The target player.
     * @throws std::runtime_error if player is blocked, target was arrested recently, or target has insufficient coins.
     */
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

    /**
     * @brief Gets the player's name.
     * @return The player's name.
     */
    std::string Player::getName() const { return this->name; }

    /**
     * @brief Gets the number of coins the player currently has.
     * @return The number of coins.
     */
    int Player::getCoins() const { return this->coins; }

    /**
     * @brief Sets the number of coins the player has.
     * @param coins The new number of coins.
     */
    void Player::setCoins(int coins) { this->coins = coins; }

    /**
     * @brief Checks if the player is currently under sanction.
     * @return true if sanctioned, false otherwise.
     */
    bool Player::isSanctioned() const { return this->sanctioned; }

    /**
     * @brief Gets the last action performed by the player.
     * @return The last action type.
     */
    ActionType Player::getLastAction() const { return this->lastAction; }

    // -------------------------------
    // Decision Callbacks
    // -------------------------------

    /**
     * @brief Sets the callback function for bribe decision making.
     * @param callback Function to call when player needs to decide about using bribe.
     */
    void Player::setBribeDecisionCallback(BribeDecisionCallback callback) {
        bribeDecisionCallback = callback;
    }

    /**
     * @brief Sets the callback function for blocking decision making.
     * @param callback Function to call when player needs to decide about blocking actions.
     */
    void Player::setBlockDecisionCallback(BlockDecisionCallback callback) {
        blockDecisionCallback = callback;
    }

    /**
     * @brief Checks if the player can use bribe (has coins and hasn't used it this turn).
     * @return true if player can use bribe, false otherwise.
     */
    bool Player::canUseBribe() const {
        return !bribeUsedThisTurn && coins >= 4;
    }

    /**
     * @brief Asks the player if they want to use bribe for a bonus action.
     * @return true if player wants to use bribe, false otherwise.
     */
    bool Player::askForBribe() const {
        if (!canUseBribe()) return false;
        if (bribeDecisionCallback) {
            return bribeDecisionCallback(*this);
        }
        return false;
    }

    /**
     * @brief Asks the player if they want to block an action.
     * @param action The action type to potentially block.
     * @param actor The player performing the action.
     * @return true if player wants to block, false otherwise.
     */
    bool Player::askForBlock(ActionType action, const Player* actor) const {
        if (blockDecisionCallback) {
            return blockDecisionCallback(*this, action, actor);  // ← השאלה
        }
        return false; // Default: don't block
    }

    // -------------------------------
    // State & Flags
    // -------------------------------

    /**
     * @brief Marks the player's last action as blocked.
     */
    void Player::blockLastAction() { this->actionBlocked = true; }

    /**
     * @brief Sets a flag to block the player's arrest ability next turn.
     */
    void Player::blockArrestNextTurn() { this->arrestBlocked = true; }

    /**
     * @brief Gets the amount of coins collected when performing tax action.
     * @return The tax amount (2 for base class, can be overridden by roles).
     */
    int Player::taxAmount() const { return 2; }

    // -------------------------------
    // Turn Logic
    // -------------------------------

    /**
     * @brief Starts the player's turn and initializes turn-related flags.
     * @throws std::runtime_error if it's not the player's turn or they must coup.
     */
    void Player::startTurn() {
        requireTurn();
        std::cout << name << " starts turn." << std::endl;
        if (coins >= 10) throw std::runtime_error("You must perform a coup!");
        bribeUsedThisTurn = false;
        lastAction = ActionType::None;
        lastActionTarget = nullptr;
        actionBlocked = false;
    }

    /**
     * @brief Ends the player's turn, processes pending actions, and advances to next player.
     */
    void Player::endTurn() {
        if (game.hasPendingAction() && game.getLastActor() == this && !actionBlocked) {
            switch (lastAction) {
                case ActionType::Tax:
                    BankManager::transferFromBank(game, *this, taxAmount());
                    break;
                case ActionType::Bribe:
                    break;
                case ActionType::Arrest:
                    if (lastActionTarget) {
                        if (lastActionTarget->getRoleName() == "Merchant")
                            BankManager::transferToBank(*lastActionTarget, game, 2);
                        else
                            BankManager::transferCoins(*lastActionTarget, *this, 1);
                        lastActionTarget->arrested = true;
                    }
                    break;
                case ActionType::Sanction: {
                    int cost = (lastActionTarget && lastActionTarget->getRoleName() == "Judge") ? 4 : 3;
                    if (coins < cost) break;
                    BankManager::transferToBank(*this, game, cost);
                    if (lastActionTarget) {
                        lastActionTarget->sanctioned = true;
                        if (lastActionTarget->getRoleName() == "Baron")
                            BankManager::transferFromBank(game, *lastActionTarget, 1);
                    }
                    break;
                }
                case ActionType::Coup:
                    if (coins < 7) break;
                    BankManager::transferToBank(*this, game, 7);
                    if (lastActionTarget) game.eliminate(*lastActionTarget);
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

    // -------------------------------
    // Game Actions
    // -------------------------------

    /**
     * @brief Performs the gather action: collect 1 coin from the bank.
     * @throws std::runtime_error if not player's turn or player is sanctioned.
     */
    void Player::gather() {
        requireTurn();
        if (sanctioned)
            throw std::runtime_error("Cannot gather, player is under sanctions");

        BankManager::transferFromBank(game, *this, 1);
        lastAction = ActionType::Gather;
        game.resolvePendingAction();

        if (!bribeUsedThisTurn && askForBribe()) {
            return;
        }

        game.nextTurn();
    }

    /**
     * @brief Performs the tax action: collect coins from the bank.
     * This action can be blocked by Governor in real-time.
     * @throws std::runtime_error if not player's turn or player is sanctioned.
     */
    void Player::tax() {
        requireTurn();
        if (sanctioned)
            throw std::runtime_error("Cannot tax, player is under sanctions");

        game.setPendingAction(this, ActionType::Tax);
        lastAction = ActionType::Tax;
        lastActionTarget = nullptr;

        game.requestImmediateResponse(this, ActionType::Tax, nullptr);
        if (actionBlocked) {
            game.resolvePendingAction();
            if (!bribeUsedThisTurn && askForBribe()) return;
            game.nextTurn();
            return;
        }

        if (!bribeUsedThisTurn && askForBribe()) {
            return;
        }

        endTurn();
    }

    /**
     * @brief Performs the bribe action: pay 4 coins for a bonus action.
     * @throws std::runtime_error if used as first action, already used this turn, or insufficient coins.
     */
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
        game.requestImmediateResponse(this, ActionType::Bribe, nullptr);
        if (actionBlocked) {
            endTurn();
            return;
        }

        lastAction = ActionType::Bribe;
        bribeUsedThisTurn = true;
    }

    /**
     * @brief Performs the arrest action: steal 1 coin from target player.
     * @param player The target player to arrest.
     * @throws std::runtime_error if validation conditions are not met.
     */
    void Player::arrest(Player &player) {
        requireTurn();
        requireAlive(player);
        requireNotSelf(player, "arrest");
        requireCanArrest(player);

        game.setPendingAction(this, ActionType::Arrest, &player);
        lastAction = ActionType::Arrest;
        lastActionTarget = &player;

        if (!bribeUsedThisTurn && askForBribe()) {
            return;
        }

        endTurn();
    }

    /**
     * @brief Performs the sanction action: prevent target from using economic actions.
     * @param player The target player to sanction.
     * @throws std::runtime_error if validation conditions are not met.
     */
    void Player::sanction(Player &player) {
        requireTurn();
        requireAlive(player);
        requireNotSelf(player, "sanction");
        requireCanSanction(player);

        game.setPendingAction(this, ActionType::Sanction, &player);
        lastAction = ActionType::Sanction;
        lastActionTarget = &player;

        if (!bribeUsedThisTurn && askForBribe()) {
            return;
        }

        endTurn();
    }

    /**
     * @brief Performs the coup action: eliminate target player from the game.
     * @param player The target player to eliminate.
     * @throws std::runtime_error if not enough coins or validation fails.
     */
    void Player::coup(Player &player) {
        requireTurn();
        requireAlive(player);
        requireNotSelf(player, "coup");
        if (coins < 7)
            throw std::runtime_error("Not enough coins to coup");

        BankManager::transferToBank(*this, game, 7);
        game.requestImmediateResponse(this, ActionType::Coup, &player);
        if (actionBlocked) {
            endTurn();
            return;
        }

        game.eliminate(player);
        lastAction = ActionType::Coup;
        lastActionTarget = &player;

        endTurn();
    }

    // -------------------------------
    // Reaction Mechanisms
    // -------------------------------

    /**
     * @brief Default blocking logic. Always returns false for base Player class.
     * @return false (no blocking).
     */
    bool Player::tryBlockAction(ActionType, Player*, Player*) {
        return false;
    }

    /**
     * @brief Default undo logic. Does nothing for base Player class.
     */
    void Player::undo(Player & /*player*/) {
        // For specific roles only
    }

}