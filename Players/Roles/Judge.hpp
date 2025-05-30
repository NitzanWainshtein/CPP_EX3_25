// Email: nitzanwa@gmail.com

#pragma once

#include "../Player.hpp"
#include "../../GameLogic/Game.hpp"
#include <string>

namespace coup {

    /**
     * @class Judge
     * @brief Judge role with legal authority and bribe blocking abilities
     * 
     * Special abilities:
     * - Can block other players' bribe actions
     * - Costs 4 coins to sanction instead of 3
     */
    class Judge : public Player {
    public:
        /**
         * @brief Constructor
         * @param game Reference to game instance
         * @param name Player's name
         */
        Judge(Game &game, const std::string &name);

        /**
         * @brief Destructor
         */
        ~Judge() override = default;
        
        /**
         * @brief Rule of Five - explicitly deleted for this role
         */
        Judge(const Judge& other) = delete;
        Judge& operator=(const Judge& other) = delete;
        Judge(Judge&& other) = delete;
        Judge& operator=(Judge&& other) = delete;

        /**
         * @brief Get role name
         * @return "Judge"
         */
        std::string getRoleName() const override;

        /**
         * @brief Block another player's bribe action
         * @param actor Player whose bribe to block
         * @throws std::runtime_error if invalid block attempt
         */
        void blockBribe(Player &actor);
    };

}