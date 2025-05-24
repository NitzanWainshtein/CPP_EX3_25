// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"
#include <string>

namespace coup {

    /**
     * @class General
     * @brief Represents the General role in the game.
     *
     * The General can block coup actions against any player (including himself) by paying 5 coins.
     * Additionally, if he was arrested in the previous turn, he gains 1 coin at the start of his turn.
     */
    class General : public Player {
    public:
        /**
         * @brief Constructs a General player.
         * @param game Reference to the game instance.
         * @param name The player's name.
         */
        General(Game &game, const std::string &name);

        General(const General& other) = delete;
        General& operator=(const General& other) = delete;
        General(General&& other) = delete;
        General& operator=(General&& other) = delete;
        /**
         * @brief Returns the role name.
         * @return A string: "General".
         */
        std::string getRoleName() const override;

        /**
         * @brief Manually blocks a coup targeting a specific player.
         * @param targetPlayer The player being saved from the coup.
         * @throws std::runtime_error if no valid coup is pending or not enough coins.
         */
        void blockCoup(Player &targetPlayer);

        /**
         * @brief Determines whether the General would block a coup.
         * Uses callback to ask the human player for decision.
         * @param actingPlayer The player performing the coup.
         * @param targetPlayer The player being targeted.
         * @return true if the General chooses to block the coup.
         */
        virtual bool shouldBlockCoup(Player& actingPlayer, Player& targetPlayer);

        /**
         * @brief Attempts to block a coup, deducts 5 coins and blocks if successful.
         * @param actingPlayer The player initiating the coup.
         * @param targetPlayer The player being targeted by the coup.
         * @return true if the coup was blocked.
         */
        bool tryBlockCoup(Player& actingPlayer, Player& targetPlayer);

        /**
         * @brief General method used by the game to allow General to react to coup actions.
         * @param action The type of action being attempted.
         * @param actor The player who performed the action.
         * @param target The target of the action.
         * @return true if the action is blocked.
         */
        bool tryBlockAction(ActionType action, Player* actor, Player* target) override;

        /**
         * @brief Called at the start of the turn.
         * If the General was arrested last turn, gains 1 coin as compensation.
         */
        void startTurn() override;
    };

}