// Email: nitzanwa@gmail.com

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../GameLogic/Game.hpp"
#include "../GameLogic/ActionType.hpp"
#include "../GameLogic/BankManager.hpp"
#include "../GameLogic/Logger.hpp"
#include <string>
#include <functional>
#include <stdexcept>

namespace coup {

    /**
     * @enum ArrestStatus
     * @brief Tracks the arrest status of a player
     */
    enum class ArrestStatus { 
        Available,    ///< Player can be arrested
        ArrestedNow,  ///< Player was just arrested this turn
        Cooldown      ///< Player is in cooldown period after arrest
    };

    /**
     * @class Player
     * @brief Base class for all player roles in the Coup game
     * 
     * This abstract class provides the common functionality for all player types
     * including basic actions, state management, and validation.
     */
    class Player {
    protected:
        Game &game;                    ///< Reference to the game instance
        std::string name;              ///< Player's name
        int coins;                     ///< Current coin count
        bool sanctioned;               ///< Whether player is sanctioned
        ArrestStatus arrestStatus;     ///< Current arrest status
        ActionType lastAction;         ///< Last action performed
        Player *lastActionTarget;      ///< Target of last action
        bool actionBlocked;            ///< Whether last action was blocked
        bool arrestBlocked;            ///< Whether arrest ability is blocked
        bool bribeUsedThisTurn;        ///< Whether bribe was used this turn

        std::function<bool(Player &)> bribeDecisionCallback;
        std::function<bool(Player &, ActionType, Player *)> blockDecisionCallback;

    public:
        /**
         * @brief Constructor
         * @param game Reference to the game instance
         * @param name Player's name
         */
        Player(Game &game, const std::string &name);
        
        /**
         * @brief Virtual destructor
         */
        virtual ~Player() = default;

        /**
         * @brief Rule of Three - using defaults as no dynamic allocation
         */
        Player(const Player& other) = default;
        Player& operator=(const Player& other) = default;

        /**
         * @brief Get player's name
         * @return Player name
         */
        std::string getName() const { return name; }
        
        /**
         * @brief Get current coin count
         * @return Number of coins
         */
        int getCoins() const { return coins; }
        
        /**
         * @brief Set coin count
         * @param amount New coin amount
         */
        void setCoins(int amount) { coins = amount; }
        
        /**
         * @brief Check if player is sanctioned
         * @return true if sanctioned
         */
        bool isSanctioned() const { return sanctioned; }
        
        /**
         * @brief Get current arrest status
         * @return ArrestStatus enum value
         */
        ArrestStatus getArrestStatus() const { return arrestStatus; }
        
        /**
         * @brief Check if arrest is blocked
         * @return true if blocked from arresting
         */
        bool isArrestBlocked() const { return arrestBlocked; }
        
        /**
         * @brief Set arrest blocked status
         * @param status New blocked status
         */
        void setArrestBlocked(bool status) { arrestBlocked = status; }
        
        /**
         * @brief Get last action performed
         * @return ActionType of last action
         */
        ActionType getLastAction() const { return lastAction; }
        
        /**
         * @brief Get target of last action
         * @return Pointer to target player or nullptr
         */
        Player* getLastActionTarget() const { return lastActionTarget; }

        /**
         * @brief Get role name
         * @return Role name as string
         */
        virtual std::string getRoleName() const { return "Player"; }
        
        /**
         * @brief Get tax amount for this role
         * @return Number of coins gained from tax
         */
        virtual int taxAmount() const { return 2; }
        
        /**
         * @brief Called at start of turn
         * @throws std::runtime_error if must coup with 10+ coins
         */
        virtual void startTurn();
        
        /**
         * @brief Called at end of turn
         * Clears turn flags and advances game
         */
        virtual void endTurn();
        
        /**
         * @brief Try to block an action
         * @param action Type of action to block
         * @param actor Player performing action
         * @param target Target of action
         * @return true if action was blocked
         */
        virtual bool tryBlockAction(ActionType action, Player *actor, Player *target);
        
        /**
         * @brief Undo an action (role-specific)
         * @param target Target of the undo
         * @throws std::runtime_error if not supported
         */
        virtual void undo(Player &target) { 
            throw std::runtime_error("Undo not supported by this role"); 
        }

        /**
         * @brief Verify it's this player's turn
         * @throws std::runtime_error if not their turn
         */
        void requireTurn() const;
        
        /**
         * @brief Verify target is alive
         * @param target Player to check
         * @throws std::runtime_error if target eliminated
         */
        void requireAlive(const Player &target) const;
        
        /**
         * @brief Verify target is not self
         * @param target Player to check
         * @param action Action name for error message
         * @throws std::runtime_error if targeting self
         */
        void requireNotSelf(const Player &target, const std::string &action) const;
        
        /**
         * @brief Verify target can be sanctioned
         * @param target Player to check
         * @throws std::runtime_error if already sanctioned
         */
        void requireCanSanction(const Player &target) const;
        
        /**
         * @brief Verify target can be arrested
         * @param target Player to check
         * @throws std::runtime_error if cannot arrest
         */
        void requireCanArrest(const Player &target) const;

        /**
         * @brief Get name of last action as string
         * @return Action name
         */
        std::string getLastActionName() const;

        /**
         * @brief Gather 1 coin from bank
         * @throws std::runtime_error if sanctioned or not turn
         */
        void gather();
        
        /**
         * @brief Collect tax (role-dependent amount)
         * @throws std::runtime_error if sanctioned or not turn
         */
        virtual void tax();
        
        /**
         * @brief Pay 4 coins for extra action
         * @throws std::runtime_error if insufficient coins or no prior action
         */
        void bribe();
        
        /**
         * @brief Take 1 coin from another player
         * @param target Player to arrest
         * @throws std::runtime_error if various conditions not met
         */
        void arrest(Player &target);
        
        /**
         * @brief Block target's economic actions
         * @param target Player to sanction
         * @throws std::runtime_error if insufficient coins
         */
        void sanction(Player &target);
        
        /**
         * @brief Eliminate another player
         * @param target Player to coup
         * @throws std::runtime_error if insufficient coins
         */
        void coup(Player &target);

        /**
         * @brief Mark last action as blocked
         */
        void blockLastAction();
        
        /**
         * @brief Ask player about bribe decision
         * @return true if player wants to bribe
         */
        bool askForBribe();
        
        /**
         * @brief Ask player about blocking decision
         * @param action Action to potentially block
         * @param actor Player performing action
         * @param target Target of action
         * @return true if player wants to block
         */
        bool askForBlock(ActionType action, Player *actor, Player *target);

        /**
         * @brief Set bribe decision callback
         * @param callback Function to call for bribe decisions
         */
        void setBribeDecisionCallback(std::function<bool(Player &)> callback) {
            bribeDecisionCallback = callback;
        }
        
        /**
         * @brief Set block decision callback
         * @param callback Function to call for block decisions
         */
        void setBlockDecisionCallback(std::function<bool(Player &, ActionType, Player *)> callback) {
            blockDecisionCallback = callback;
        }
        
        /**
         * @brief Check if can use bribe
         * @return true if bribe is available
         */
        bool canUseBribe() const {
            return !bribeUsedThisTurn && coins >= 4 && lastAction != ActionType::None;
        }
        
        /**
         * @brief Check if already bribed this turn
         * @return true if bribe was used
         */
        bool hasBribedThisTurn() const {
            return bribeUsedThisTurn;
        }
    
        /**
         * @brief Clear turn-specific flags
         */
        void clearTurnFlags() {
            sanctioned = false;
            arrestBlocked = false;
        }
    };

}

#endif // PLAYER_HPP