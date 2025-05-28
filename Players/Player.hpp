// Email: nitzanwa@gmail.com

#pragma once

#include <string>
#include <functional>

namespace coup {

    class Game;               // forward declaration
    enum class ActionType;    // forward declaration

    /**
     * @class Player
     * @brief Abstract base class representing a player in the Coup game.
     *
     * Each derived class implements a specific role with unique capabilities.
     */
    class Player {
    public:
        // Callback function types
        using BribeDecisionCallback = std::function<bool(const Player&)>;
        using BlockDecisionCallback = std::function<bool(const Player&, ActionType, const Player*)>;

    protected:
        Game &game;                  ///< Reference to the game instance.
        std::string name;            ///< Player's name.
        int coins;                   ///< Amount of coins the player has.
        bool sanctioned;             ///< If player is under sanction.
        bool arrested;               ///< If player was arrested last turn.

        ActionType lastAction;       ///< Last action performed by the player.
        Player *lastActionTarget;    ///< Target of the last action.
        bool actionBlocked;          ///< Whether the last action was blocked.
        bool bribeUsedThisTurn;      ///< Flag for bribe usage this turn.
        bool arrestBlocked;          ///< Flag for arrest block by Spy.
        static bool guiMode;
        // Decision callbacks
        BribeDecisionCallback bribeDecisionCallback;
        BlockDecisionCallback blockDecisionCallback;

        /**
         * @brief Verifies that it's this player's turn.
         * @throws std::runtime_error if it's not the player's turn.
         */
        void requireTurn() const;

        /**
         * @brief Verifies that the target player is alive.
         * @param target The player to check.
         * @throws std::runtime_error if the target is not alive.
         */
        void requireAlive(const Player &target) const;

        /**
         * @brief Verifies that the other player is not this player.
         * @param other The player to compare with.
         * @param action Action name for the error message.
         * @throws std::runtime_error if trying to act on self.
         */
        void requireNotSelf(const Player &other, const std::string &action) const;

        /**
         * @brief Validates whether the player can perform sanction on the target.
         * @param target The target player.
         * @throws std::runtime_error if conditions are not met.
         */
        void requireCanSanction(const Player &target) const;

        /**
         * @brief Validates whether the player can arrest the target.
         * @param target The target player.
         * @throws std::runtime_error if conditions are not met.
         */
        void requireCanArrest(const Player &target) const;

        /**
         * @brief Asks if player wants to use bribe for bonus action.
         * @return true if player wants to bribe.
         */
        bool askForBribe() const;

        /**
         * @brief Asks if player wants to block an action.
         * @param action The action type to potentially block.
         * @param actor The player performing the action.
         * @return true if player wants to block.
         */
        bool askForBlock(ActionType action, const Player* actor) const;

    public:
        /**
         * @brief Constructs a new Player.
         * @param game Reference to the game instance.
         * @param name Player's name.
         */
        Player(Game &game, const std::string &name);

        /**
         * @brief Virtual destructor.
         */
        virtual ~Player();

        Player(const Player& other) = delete;
        Player& operator=(const Player& other) = delete;
        Player(Player&& other) = delete;
        Player& operator=(Player&& other) = delete;

        // -----------------------
        // Basic Info & State
        // -----------------------

        std::string getName() const;
        int getCoins() const;
        void setCoins(int coins);
        bool isSanctioned() const;
        ActionType getLastAction() const;

        void blockLastAction();
        void blockArrestNextTurn();

        // -----------------------
        // NEW: GUI Helper Methods
        // -----------------------

        /**
         * @brief Resets turn state manually (for GUI forced coup with 10+ coins).
         */
        void resetTurnState();
        static void setGuiMode(bool mode) { guiMode = mode; }
        static bool isGuiMode() { return guiMode; }
        /**
         * @brief Gets the current bribe callback (for GUI override).
         */
        BribeDecisionCallback getBribeCallback() const;

        /**
         * @brief Temporarily overrides bribe callback (for GUI).
         */
        void overrideBribeCallback(BribeDecisionCallback callback);

        // -----------------------
        // Decision Callbacks
        // -----------------------

        /**
         * @brief Sets callback for bribe decisions.
         * @param callback Function to call when deciding about bribe.
         */
        void setBribeDecisionCallback(BribeDecisionCallback callback);

        /**
         * @brief Sets callback for blocking decisions.
         * @param callback Function to call when deciding about blocking.
         */
        void setBlockDecisionCallback(BlockDecisionCallback callback);

        /**
         * @brief Checks if player can use bribe.
         * @return true if player can afford bribe and hasn't used it this turn.
         */
        bool canUseBribe() const;

        // -----------------------
        // Turn Management
        // -----------------------

        virtual void startTurn();
        void endTurn();

        // -----------------------
        // Core Actions
        // -----------------------

        void gather();
        virtual void tax();
        virtual int taxAmount() const;
        virtual void bribe();
        virtual void arrest(Player &player);
        virtual void sanction(Player &player);
        virtual void coup(Player &player);

        // -----------------------
        // Reaction & Role Info
        // -----------------------

        virtual bool tryBlockAction(ActionType action, Player* actor, Player* target);
        virtual void undo(Player &player);
        virtual std::string getRoleName() const = 0;
        /**
         * @brief Checks if player has used bribe this turn.
         * @return true if player has bribed this turn.
         */
        bool hasBribedThisTurn() const;

    };

}