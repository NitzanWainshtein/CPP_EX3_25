// Email: nitzanwa@gmail.com

#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>
#include <memory>
#include "ActionType.hpp"

namespace coup {

    class Player;  // forward declaration

    /**
     * @class Game
     * @brief Main game controller class that manages the Coup game state and flow
     * 
     * This class handles:
     * - Player management and turn order
     * - Bank coins tracking
     * - Game state validation
     * - Action blocking mechanisms
     * - Win condition checking
     */
    class Game {
    private:
        std::vector<Player *> player_list;     ///< List of all players in the game
        size_t current_turn_index;             ///< Index of the current player's turn
        int bankCoins;                         ///< Number of coins in the bank
        Player *pendingActionActor;            ///< Player who performed the last action
        ActionType pendingActionType;          ///< Type of the last action performed
        Player *pendingActionTarget;           ///< Target of the last action (if any)
        std::string lastWinnerName;            ///< Name of the winner (cached for game reset)
        bool isConsoleMode;                    ///< Whether game is in console mode or GUI mode

    public:
        /**
         * @brief Default constructor - initializes a new game
         * Sets bank to 200 coins and prepares for player registration
         */
        Game();
        
        /**
         * @brief Destructor - cleans up game resources
         */
        ~Game();
        
        /**
         * @brief Rule of Three - explicitly deleted
         */
        Game(const Game& other) = delete;
        Game& operator=(const Game& other) = delete;

        /**
         * @brief Adds a player to the game
         * @param player Pointer to the player to add
         * @throws std::runtime_error if game is full (6 players) or name exists
         */
        void addPlayer(Player *player);
        
        /**
         * @brief Gets the name of the player whose turn it is
         * @return Current player's name
         */
        std::string turn() const;
        
        /**
         * @brief Advances to the next player's turn
         * Skips eliminated players automatically
         */
        void nextTurn();
        
        /**
         * @brief Gets list of all active player names
         * @return Vector of player names still in the game
         */
        std::vector<std::string> players() const;

        /**
         * @brief Sets the number of coins in the bank
         * @param coins Number of coins to set
         * @throws std::runtime_error if coins is negative
         */
        void setBankCoins(int coins);
        
        /**
         * @brief Gets the current number of coins in the bank
         * @return Number of coins in bank
         */
        int getBankCoins() const;

        /**
         * @brief Checks if a player is still active in the game
         * @param player Player to check
         * @return true if player is alive, false if eliminated
         */
        bool isAlive(const Player &player) const;
        
        /**
         * @brief Eliminates a player from the game
         * @param player Player to eliminate
         */
        void eliminate(Player &player);
        
        /**
         * @brief Checks if the game has ended
         * @return true if only one player remains
         */
        bool isGameOver() const;
        
        /**
         * @brief Gets the winner's name
         * @return Name of the winning player
         * @throws std::runtime_error if game is not over
         */
        std::string winner() const;

        /**
         * @brief Resets the game to initial state
         * Clears all players and resets bank to 200 coins
         */
        void resetGame();

        /**
         * @brief Records a pending action for potential blocking
         * @param actor Player performing the action
         * @param actionType Type of action being performed
         * @param target Target player (optional)
         */
        void setPendingAction(Player *actor, ActionType actionType, Player *target = nullptr);
        
        /**
         * @brief Checks if there's a pending action
         * @return true if an action is pending
         */
        bool hasPendingAction() const;
        
        /**
         * @brief Gets the player who performed the last action
         * @return Pointer to the last actor
         */
        Player *getLastActor() const;
        
        /**
         * @brief Clears the pending action
         */
        void resolvePendingAction();

        /**
         * @brief Requests immediate response for blocking
         * @param actor Player performing the action
         * @param action Type of action
         * @param target Target player (if any)
         */
        void requestImmediateResponse(Player *actor, ActionType action, Player *target);

        /**
         * @brief Checks if a player name already exists
         * @param name Name to check
         * @return true if name exists
         */
        bool nameExists(const std::string &name) const;

        /**
         * @brief Gets all players still alive
         * @return Vector of pointers to alive players
         */
        std::vector<Player*> getAllAlivePlayers() const;
        
        /**
         * @brief Gets the current player
         * @return Pointer to current player or nullptr
         */
        Player* getCurrentPlayer() const;
        
        /**
         * @brief Converts action type to string
         * @param action Action type to convert
         * @return String representation of action
         */
        std::string getActionName(ActionType action) const;
        
        /**
         * @brief Checks if any player can block an action
         * @param actor Player performing the action
         * @param action Type of action
         * @param target Target player (optional)
         * @return true if action was blocked
         */
        bool checkForBlocking(Player* actor, ActionType action, Player* target = nullptr);
        
        /**
         * @brief Checks if a specific player can block an action
         * @param blocker Player who might block
         * @param action Type of action
         * @param actor Player performing the action
         * @param target Target of the action
         * @return true if blocker can block this action
         */
        bool canPlayerBlock(Player* blocker, ActionType action, Player* actor, Player* target) const;
        
        /**
         * @brief Executes a block action
         * @param blocker Player performing the block
         * @param action Type of action being blocked
         * @param actor Original actor
         * @param target Original target
         */
        void executeBlock(Player* blocker, ActionType action, Player* actor, Player* target);

        /**
         * @brief Sets console mode on/off
         * @param console true for console mode, false for GUI mode
         */
        void setConsoleMode(bool console) { isConsoleMode = console; }
        
        /**
         * @brief Gets current mode
         * @return true if in console mode
         */
        bool getConsoleMode() const { return isConsoleMode; }
    };

}

#endif // GAME_HPP