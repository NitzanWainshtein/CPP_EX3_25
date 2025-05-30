// Email: nitzanwa@gmail.com

#pragma once

#include "../../GameLogic/Game.hpp"
#include "../Player.hpp"
#include <string>

namespace coup {

    /**
     * @class Baron
     * @brief Baron role with investment and economic resilience abilities
     * 
     * Special abilities:
     * - Can invest 3 coins to receive 6 coins back
     * - Receives 1 coin compensation when sanctioned
     */
    class Baron : public Player {
    public:
        /**
         * @brief Constructor
         * @param game Reference to game instance
         * @param name Player's name
         */
        Baron(Game &game, const std::string &name);

        /**
         * @brief Destructor
         */
        ~Baron() override = default;
        
        /**
         * @brief Rule of Three - explicitly deleted
         */
        Baron(const Baron& other) = delete;
        Baron& operator=(const Baron& other) = delete;

        /**
         * @brief Invest 3 coins to receive 6 coins
         * @throws std::runtime_error if insufficient coins or not turn
         */
        void invest();
        
        /**
         * @brief Get role name
         * @return "Baron"
         */
        std::string getRoleName() const override;
    };

}