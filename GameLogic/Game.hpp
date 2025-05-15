// Email: nitzanwa@gmail.com

#pragma once

#include <string>
#include <vector>
#include "../Players/Player.hpp"

namespace coup {

    class Player;

    /**
     * @enum ActionType
     * @brief Enum representing all possible player actions in the game.
     */
    enum class ActionType {
        None,      ///< No action (default).
        Tax,       ///< Collect tax.
        Bribe,     ///< Pay for extra move.
        Coup,      ///< Eliminate another player.
        Sanction,  ///< Prevent a player from using economic actions.
        Arrest,    ///< Steal a coin from another player.
        Gather,    ///< Take 1 coin from the bank.
        Invest     ///< Special action for Baron.
    };

    /**
     * @class Game
     * @brief Represents the state and logic of a single Coup game.
     * 
     * Manages turn order, player list, bank, pending actions,
     * and responses like blocking actions.
     */
    class Game {
    private:
        std::vector<Player*> player_list; ///< List of players in game (nullptr if eliminated).
        int current_turn_index;           ///< Index of current player's turn.
        int bank;                         ///< Central coin pool.

        // Pending action tracking
        Player* lastActingPlayer;         ///< Player who performed the last pending action.
        Player* lastActionTarget;         ///< Target of the last pending action (if any).
        ActionType lastActionType;        ///< Type of the last pending action.
        bool actionPending;               ///< Whether an action is currently pending.

    public:
        /**
         * @brief Constructs a new Game with default state.
         */
        Game();

        /**
         * @brief Checks if a player name already exists.
         * @param name The name to check.
         * @return True if name exists, false otherwise.
         */
        bool nameExists(const std::string& name) const;

        /**
         * @brief Adds a player to the game.
         * @param player Pointer to the player object.
         * @throws std::runtime_error if the name already exists.
         */
        void addPlayer(Player* player);

        /**
         * @brief Returns names of all active (non-eliminated) players.
         */
        std::vector<std::string> players() const;

        /**
         * @brief Returns the name of the player whose turn it is.
         */
        std::string turn() const;

        /**
         * @brief Advances to the next active player's turn.
         */
        void nextTurn();

        /**
         * @brief Eliminates a player from the game.
         * @param player The player to eliminate.
         */
        void eliminate(Player& player);

        /**
         * @brief Deletes all players and resets game state.
         */
        void resetPlayers();

        /**
         * @brief Gets the number of coins currently in the bank.
         */
        int getBank() const;

        /**
         * @brief Adds coins to the bank.
         * @param amount Number of coins to add.
         * @throws std::runtime_error if amount is negative.
         */
        void addToBank(int amount);

        /**
         * @brief Removes coins from the bank.
         * @param amount Number of coins to remove.
         * @throws std::runtime_error if bank does not have enough coins.
         */
        void takeFromBank(int amount);

        /**
         * @brief Checks if the game has ended.
         * @return True if only one player remains.
         */
        bool isGameOver() const;

        /**
         * @brief Checks if a given player is still in the game.
         */
        bool isAlive(const Player& player) const;

        /**
         * @brief Returns the name of the winner.
         * @throws std::runtime_error if the game is not over.
         */
        std::string winner() const;

        // -----------------------
        // Pending Action Logic
        // -----------------------

        /**
         * @brief Sets a pending action for later resolution.
         * @param actor The player performing the action.
         * @param action The type of action.
         * @param target Optional target player.
         */
        void setPendingAction(Player* actor, ActionType action, Player* target = nullptr);

        /**
         * @brief Clears the current pending action.
         */
        void resolvePendingAction();

        /**
         * @brief Checks if there is an unresolved pending action.
         */
        bool hasPendingAction() const;

        /**
         * @brief Gets the player who last initiated a pending action.
         */
        Player* getLastActor() const;

        /**
         * @brief Gets the type of the last pending action.
         */
        ActionType getLastActionType() const;

        /**
         * @brief Gets the target player of the last pending action.
         */
        Player* getLastTarget() const;

        /**
         * @brief Checks if any players wish to block the given action.
         * @param actor The player who initiated the action.
         * @param action The type of action to potentially block.
         * @param target Optional target of the action.
         */
        void requestImmediateResponse(Player *actor, ActionType action, Player *target);
    };

}
