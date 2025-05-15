// Email: nitzanwa@gmail.com

#include "Spy.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    /**
     * @brief Constructs a Spy player.
     * @param game Reference to the game instance.
     * @param name The player's name.
     */
    Spy::Spy(Game &game, const std::string &name)
        : Player(game, name) {}

    /**
     * @brief Reveals the number of coins the target player currently holds.
     * @param target The player to inspect.
     * @return Number of coins the target has.
     */
    int Spy::peekCoins(const Player &target) const {
        return target.getCoins();
    }

    /**
     * @brief Prevents a target player from using arrest on their next turn.
     *
     * Sets a flag on the target, which is later checked in their own arrest logic.
     * @param target The player to block.
     */
    void Spy::blockNextArrest(Player &target) {
        target.blockArrestNextTurn();
    }

    /**
     * @brief Returns the name of the role.
     * @return A string: "Spy"
     */
    std::string Spy::getRoleName() const {
        return "Spy";
    }

}
