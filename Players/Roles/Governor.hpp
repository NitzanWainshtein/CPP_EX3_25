// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"

namespace coup {

    /**
     * @class Governor
     * @brief Represents the Governor role in the game.
     *
     * The Governor collects 3 coins instead of 2 when performing a tax action.
     * Additionally, the Governor can undo a pending tax action performed by another player,
     * provided it is the last action made.
     */
    class Governor : public Player {
    public:
        /**
         * @brief Constructs a Governor player.
         * @param game Reference to the game instance.
         * @param name The player's name.
         */
        Governor(Game &game, const std::string &name);

        Governor(const Governor& other) = delete;
        Governor& operator=(const Governor& other) = delete;
        Governor(Governor&& other) = delete;
        Governor& operator=(Governor&& other) = delete;


        /**
         * @brief Performs a special tax action worth 3 coins.
         * The coins are collected during endTurn().
         */
        void tax() override;

        /**
         * @brief Returns the tax amount for the Governor (3 coins).
         */
        int taxAmount() const override;

        /**
         * @brief Cancels a pending tax action performed by the last acting player.
         * Can only be used by the Governor.
         *
         * @param player The player whose tax action is being undone.
         */
        void undo(Player &player) override;

        /**
         * @brief Attempts to block tax actions from other players in real-time.
         * @param action The action type being attempted.
         * @param actor The player performing the action.
         * @param target The target of the action (unused for tax).
         * @return true if the action is blocked, false otherwise.
         */
        bool tryBlockAction(ActionType action, Player* actor, Player* target) override;

        /**
         * @brief Returns the name of the role.
         * @return A string: "Governor"
         */
        std::string getRoleName() const override;
    };

}