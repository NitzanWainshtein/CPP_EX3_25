// Email: nitzanwa@gmail.com

#ifndef GENERAL_HPP
#define GENERAL_HPP

#include "../Player.hpp"

namespace coup {

    /**
     * @class General
     * @brief General role with military authority and defensive capabilities
     * 
     * Special abilities:
     * - Can block coup attempts by paying 5 coins
     * - Receives 1 coin compensation when arrested
     */
    class General : public Player {
    public:
        /**
         * @brief Constructor
         * @param game Reference to game instance
         * @param name Player's name
         */
        General(Game &game, const std::string &name);
        
        /**
         * @brief Destructor
         */
        ~General() override = default;
        
        /**
         * @brief Rule of Three - explicitly deleted
         */
        General(const General &other) = delete;
        General &operator=(const General &other) = delete;
        
        /**
         * @brief Get role name
         * @return "General"
         */
        std::string getRoleName() const override { return "General"; }

        /**
         * @brief Try to block an action (coup specifically)
         * @param action Type of action
         * @param actor Player performing action
         * @param target Target of action
         * @return true if blocked
         */
        bool tryBlockAction(ActionType action, Player *actor, Player *target) override;
        
        /**
         * @brief Decide whether to block a coup
         * @param actingPlayer Player performing coup
         * @param targetPlayer Target of coup
         * @return true if should block
         */
        bool shouldBlockCoup(Player &actingPlayer, Player &targetPlayer);
        
        /**
         * @brief Execute coup block by paying 5 coins
         * @param targetPlayer Player being saved from coup
         * @throws std::runtime_error if insufficient coins
         */
        void blockCoup(Player &targetPlayer);
        
        /**
         * @brief Called at start of turn
         */
        void startTurn() override;
    };

}

#endif // GENERAL_HPP