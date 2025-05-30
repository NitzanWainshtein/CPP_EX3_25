// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"
#include <string>

namespace coup {

    /**
     * @class Governor
     * @brief Governor role with enhanced taxation and tax blocking abilities
     * 
     * Special abilities:
     * - Collects 3 coins from tax instead of 2
     * - Can block other players' tax actions
     */
    class Governor : public Player {
    public:
        /**
         * @brief Constructor
         * @param game Reference to game instance
         * @param name Player's name
         */
        Governor(Game &game, const std::string &name);

        /**
         * @brief Destructor
         */
        ~Governor() override = default;
        
        /**
         * @brief Rule of Five - explicitly deleted for this role
         */
        Governor(const Governor& other) = delete;
        Governor& operator=(const Governor& other) = delete;
        Governor(Governor&& other) = delete;
        Governor& operator=(Governor&& other) = delete;

        /**
         * @brief Get role name
         * @return "Governor"
         */
        std::string getRoleName() const override;

        /**
         * @brief Enhanced tax collection - gets 3 coins
         * @throws std::runtime_error if sanctioned or not turn
         */
        void tax() override;

        /**
         * @brief Block another player's tax action
         * @param actor Player whose tax to block
         * @throws std::runtime_error if invalid block attempt
         */
        void blockTax(Player &actor);
    };

}