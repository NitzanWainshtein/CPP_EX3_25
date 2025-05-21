// Email: nitzanwa@gmail.com

#include "General.hpp"
#include "../../GameLogic/Game.hpp"
#include "../../GameLogic/BankManager.hpp"
#include <stdexcept>

namespace coup {

    /**
     * @brief Constructs a General player with the given name.
     * @param game Reference to the current game.
     * @param name The name of the player.
     */
    General::General(Game &game, const std::string &name)
        : Player(game, name) {}

    /**
     * @brief Called at the start of the General's turn.
     *
     * If the General was arrested last turn, receives 1 coin as compensation.
     */
    void General::startTurn() {
        Player::startTurn();
        if (arrested) {
            BankManager::transferFromBank(game, *this, 1);
        }
    }

    /**
     * @brief Explicitly blocks a coup targeting the given player.
     *
     * This is a stricter version used only for manual override (not via tryBlockAction).
     *
     * @param targetPlayer The player targeted by the coup.
     * @throws std::runtime_error if there is no coup pending or not enough coins.
     */
    void General::blockCoup(Player &targetPlayer) {
        if (!game.hasPendingAction()) {
            throw std::runtime_error("No action to block.");
        }
        if (game.getLastActionType() != ActionType::Coup) {
            throw std::runtime_error("Last action is not a coup.");
        }
        if (game.getLastTarget() != &targetPlayer) {
            throw std::runtime_error("This coup is not targeted at the specified player.");
        }
        if (coins < 5) {
            throw std::runtime_error("General does not have enough coins to block the coup.");
        }

        BankManager::transferToBank(*this, game, 5);
        game.getLastActor()->blockLastAction();
    }

    /**
     * @brief Determines whether the General should block a given coup.
     * Uses the blockDecisionCallback to ask the human player.
     * @param actingPlayer The player performing the coup.
     * @param targetPlayer The player being targeted by the coup.
     * @return true if the General wants to block this coup.
     */
    bool General::shouldBlockCoup(Player &actingPlayer, Player &targetPlayer) {
        return askForBlock(ActionType::Coup, &actingPlayer);
    }

    /**
     * @brief Attempts to block a coup targeting a player.
     *
     * Pays 5 coins to the bank if successful and blocks the coup action.
     * The coins are not refunded even if the block is later undone.
     *
     * @param actingPlayer The player who initiated the coup.
     * @param targetPlayer The player being targeted by the coup.
     * @return true if the coup was successfully blocked.
     */
    bool General::tryBlockCoup(Player &actingPlayer, Player &targetPlayer) {
        if (&targetPlayer != this && !this->game.isAlive(targetPlayer)) return false;
        if (coins < 5) return false;
        if (!shouldBlockCoup(actingPlayer, targetPlayer)) return false;

        BankManager::transferToBank(*this, game, 5);
        actingPlayer.blockLastAction();
        return true;
    }

    /**
     * @brief General method used by the game to allow General to react to coup actions.
     * @param action The type of action being attempted.
     * @param actor The player who performed the action.
     * @param target The target of the action.
     * @return true if the action is blocked.
     */
    bool General::tryBlockAction(ActionType action, Player* actor, Player* target) {
        if (action == ActionType::Coup && actor != nullptr && target != nullptr) {
            return tryBlockCoup(*actor, *target);
        }
        return false;
    }

    /**
     * @brief Returns the name of the role.
     * @return A string: "General"
     */
    std::string General::getRoleName() const {
        return "General";
    }

}