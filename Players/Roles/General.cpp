#include "General.hpp"
#include "../../GameLogic/BankManager.hpp"
#include "../../GameLogic/Logger.hpp"
#include "../../GameLogic/Game.hpp"
#include <stdexcept>

namespace coup {

    General::General(Game &game, const std::string &name)
        : Player(game, name) {}

    void General::startTurn() {
        Logger::log("Starting turn for General: " + name);
        Player::startTurn();
        if (arrestStatus != ArrestStatus::Available) {
            Logger::log(name + " is under arrest and cannot act normally.");
        }
    }

    void General::blockCoup(Player &targetPlayer) {
        Logger::log(name + " is attempting to block a coup on " + targetPlayer.getName());

        if (coins < 5) {
            Logger::log("General does not have enough coins to block the coup");
            throw std::runtime_error("General needs 5 coins to block coup");
        }

        BankManager::transferToBank(*this, game, 5);

        if (!game.isAlive(targetPlayer)) {
            Logger::log("Cannot block coup: target player is not alive");
            throw std::runtime_error("Cannot block coup on inactive player");
        }
        if (!game.hasPendingAction()) {
            Logger::log("No pending coup action to block");
            throw std::runtime_error("No coup action to block");
        }

        Player *lastActor = game.getLastActor();
        if (!lastActor) {
            Logger::log("No last actor found for blocking");
            throw std::runtime_error("No last actor found");
        }
        if (lastActor->getLastAction() != ActionType::Coup) {
            Logger::log("Last action was not coup, cannot block");
            throw std::runtime_error("Last action was not coup");
        }
        if (lastActor->getLastActionTarget() != &targetPlayer) {
            Logger::log("Last target does not match the coup block target");
            throw std::runtime_error("Last target does not match");
        }

        lastActor->blockLastAction();
        Logger::log(name + " successfully blocked coup against " + targetPlayer.getName() + " (cost 5 coins)");
    }

    bool General::shouldBlockCoup(Player &actingPlayer, Player &targetPlayer) {
        Logger::log(name + " is deciding whether to block coup from " + actingPlayer.getName() + " on " + targetPlayer.getName());
        return askForBlock(ActionType::Coup, &actingPlayer, &targetPlayer);
    }

    bool General::tryBlockAction(ActionType action, Player *actor, Player *target) {
        Logger::log(name + " evaluating if can block action: " + std::to_string(static_cast<int>(action)));
        if (action == ActionType::Coup && shouldBlockCoup(*actor, *target)) {
            blockCoup(*target);
            return true;
        }
        return Player::tryBlockAction(action, actor, target);
    }

}
