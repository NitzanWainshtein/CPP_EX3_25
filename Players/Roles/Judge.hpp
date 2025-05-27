// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"

namespace coup {

    /**
     * @class Judge
     * @brief Represents the Judge role in the game.
     *
     * The Judge can block bribe actions from other players. This happens in real-time
     * via the tryBlockAction mechanism. Bribe costs the initiating player coins,
     * and when blocked, the coins are still lost.
     */
    class Judge : public Player {
    public:
        /**
         * @brief Constructs a Judge player.
         * @param game Reference to the game instance.
         * @param name The name of the player.
         */
        Judge(Game &game, const std::string &name);

        Judge(const Judge& other) = delete;
        Judge& operator=(const Judge& other) = delete;
        Judge(Judge&& other) = delete;
        Judge& operator=(Judge&& other) = delete;

        /**
         * @brief Returns the name of the role.
         * @return A string: "Judge".
         */
        std::string getRoleName() const override;

        /**
         * @brief Indicates whether the Judge wants to block a bribe action.
         * Uses the blockDecisionCallback to ask the human player.
         * @param actingPlayer The player who attempted to bribe.
         * @return true if the Judge chooses to block.
         */
        virtual bool shouldBlockBribe(Player &actingPlayer);

        /**
         * @brief Attempts to block a bribe action.
         * @param actingPlayer The player who attempted the bribe.
         * @return true if the bribe was blocked.
         */
        bool tryBlockBribe(Player &actingPlayer);

        /**
         * @brief General method used by the game to allow Judge to react to actions.
         * @param action The type of action being attempted.
         * @param actor The player who performed the action.
         * @param target Optional target of the action.
         * @return true if the action is blocked.
         */
        bool tryBlockAction(ActionType action, Player* actor, Player* target) override;

        /**
         * @brief Unused for Judge; override exists for interface completeness.
         * @param player Unused parameter (marked to avoid warning).
         */
        void undo(Player & /*player*/) override {}
    };

}