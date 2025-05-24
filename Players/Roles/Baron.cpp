// Email: nitzanwa@gmail.com

#include "Baron.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {
    /**
     * @brief Constructs a Baron role for the specified player.
     *
     * @param game Reference to the game instance.
     * @param name The name of the player.
     */
    Baron::Baron(Game &game, const std::string &name)
        : Player(game, name) {
    }

    /**
     * @brief Performs the Baron's special ability: invest.
     *
     * The player pays 3 coins to the bank and receives 6 coins in return.
     * This action can only be done during the player's turn and if they have at least 3 coins.
     *
     * @throws std::runtime_error if it's not the player's turn.
     * @throws std::runtime_error if the player has fewer than 3 coins.
     */
    void Baron::invest() {
        if (game.turn() != name) throw std::runtime_error("Not your turn");
        if (coins < 3) throw std::runtime_error("Not enough coins to invest");

        BankManager::transferToBank(*this, game, 3);
        BankManager::transferFromBank(game, *this, 6);

        lastAction = ActionType::Invest;
        if (!bribeUsedThisTurn && askForBribe()) {
            return;
        }
        endTurn();
    }

    /**
     * @brief Returns the role name of the player.
     * @return A string representing the role: "Baron".
     */
    std::string Baron::getRoleName() const {
        return "Baron";
    }
}
