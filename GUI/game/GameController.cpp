// GUI/game/GameController.cpp
#include "GameController.hpp"
#include "../../GameLogic/PlayerFactory.hpp"
#include "../../Players/Roles/Governor.hpp"
#include "../../Players/Roles/Spy.hpp"
#include "../../Players/Roles/Baron.hpp"
#include "../../Players/Roles/General.hpp"
#include "../../Players/Roles/Judge.hpp"
#include "../../Players/Roles/Merchant.hpp"
#include <algorithm>
#include <iostream>

namespace coup {
namespace gui {

GameController::~GameController() {
    resetGame();
}

bool GameController::createGame(const std::vector<std::string>& playerNames) {
    try {
        // Check for duplicate names
        std::vector<std::string> namesCopy = playerNames;
        std::sort(namesCopy.begin(), namesCopy.end());
        if (std::adjacent_find(namesCopy.begin(), namesCopy.end()) != namesCopy.end()) {
            notifyError("Duplicate player names found!");
            return false;
        }

        // Reset any existing game
        resetGame();

        // Create new game
        game = std::make_unique<Game>();

        // Enable GUI mode
        Player::setGuiMode(true);

        // Create players with random roles - KEEP ORIGINAL ORDER
        for (const auto& name : playerNames) {
            try {
                Player* player = randomPlayer(*game, name);
                players.push_back(player);
                notifyMessage(name + " joined as " + player->getRoleName());
            } catch (const std::exception& e) {
                notifyError("Failed to create player " + name + ": " + std::string(e.what()));
                resetGame();
                return false;
            }
        }

        // Setup callbacks
        setupPlayerCallbacks();

        // Update current player
        updateCurrentPlayer();

        notifyMessage("Game started! " + getCurrentPlayerName() + " goes first.");
        notifyStateChange();

        return true;

    } catch (const std::exception& e) {
        notifyError("Failed to create game: " + std::string(e.what()));
        resetGame();
        return false;
    }
}

void GameController::resetGame() {
    // Reset state first
    currentPlayer = nullptr;
    turnInProgress = false;
    waitingForBribeDecision = false;

    // Clean up players
    for (Player* p : players) {
        delete p;
    }
    players.clear();

    // Reset game
    game.reset();
}

void GameController::setupPlayerCallbacks() {
    for (auto* player : players) {
        // Bribe decision callback
        player->setBribeDecisionCallback([this, player](const Player& p) -> bool {
            if (&p == currentPlayer && currentPlayer->canUseBribe()) {
                waitingForBribeDecision = true;
                notifyMessage(currentPlayer->getName() + " can use bribe for a bonus action.");
                notifyStateChange();
                return true; // Prevent automatic turn end
            }
            return false;
        });

        // Block decision callback - DISABLED for GUI
        player->setBlockDecisionCallback([](const Player&, ActionType, const Player*) -> bool {
            // In GUI mode, never auto-block
            // Blocking should be handled through UI
            return false;
        });
    }
}

void GameController::updateCurrentPlayer() {
    if (!game) {
        currentPlayer = nullptr;
        return;
    }

    try {
        std::string currentName = game->turn();
        currentPlayer = nullptr;

        for (auto* p : players) {
            if (p && game->isAlive(*p) && p->getName() == currentName) {
                currentPlayer = p;
                break;
            }
        }
    } catch (...) {
        currentPlayer = nullptr;
    }
}

bool GameController::isGameOver() const {
    return game && game->isGameOver();
}

std::string GameController::getCurrentPlayerName() const {
    if (!game) return "";
    try {
        return game->turn();
    } catch (...) {
        return "";
    }
}

std::string GameController::getWinner() const {
    if (!game || !game->isGameOver()) return "";
    try {
        return game->winner();
    } catch (...) {
        return "";
    }
}

int GameController::getBankCoins() const {
    return game ? game->getBank() : 0;
}

std::vector<std::string> GameController::getPlayerNames() const {
    if (!game) return std::vector<std::string>();

    // Return ALL player names in original order, not just alive ones
    std::vector<std::string> names;
    for (const auto* p : players) {
        if (p) {
            names.push_back(p->getName());
        }
    }
    return names;
}

Player* GameController::getPlayer(const std::string& name) const {
    for (auto* p : players) {
        if (p && p->getName() == name) {
            return p;
        }
    }
    return nullptr;
}

bool GameController::isPlayerAlive(const std::string& name) const {
    if (!game) return false;
    Player* p = getPlayer(name);
    return p && game->isAlive(*p);
}

bool GameController::canStartTurn() const {
    return currentPlayer && !turnInProgress && !isGameOver() && !waitingForBribeDecision;
}

void GameController::startTurn() {
    if (!canStartTurn()) {
        notifyError("Cannot start turn now!");
        return;
    }

    try {
        currentPlayer->startTurn();
        turnInProgress = true;
        notifyMessage(currentPlayer->getName() + " started their turn.");
        notifyStateChange();
    } catch (const std::exception& e) {
        // Check if it's the 10 coin rule
        if (currentPlayer && currentPlayer->getCoins() >= 10) {
            turnInProgress = true; // Allow coup selection
            notifyMessage("You must coup! (10+ coins)");
            notifyStateChange();
        } else {
            notifyError(e.what());
        }
    }
}

void GameController::endTurn() {
    if (!currentPlayer || !turnInProgress) return;

    try {
        currentPlayer->endTurn();
        turnInProgress = false;
        waitingForBribeDecision = false;

        updateCurrentPlayer();

        if (isGameOver()) {
            notifyMessage("Game Over! Winner: " + getWinner());
        } else {
            notifyMessage("Turn ended. Now it's " + getCurrentPlayerName() + "'s turn.");
        }

        notifyStateChange();
    } catch (const std::exception& e) {
        notifyError(e.what());
        turnInProgress = false;
        waitingForBribeDecision = false;
    }
}

// Action implementations
void GameController::performGather() {
    if (!currentPlayer || !turnInProgress) return;

    try {
        currentPlayer->gather();
        notifyMessage(currentPlayer->getName() + " gathered 1 coin.");
        notifyStateChange();

        // Don't auto-end turn - wait for user to click End Turn
        // unless bribe decision is pending
    } catch (const std::exception& e) {
        notifyError(e.what());
    }
}

void GameController::performTax() {
    if (!currentPlayer || !turnInProgress) return;

    try {
        int taxAmount = currentPlayer->taxAmount();
        currentPlayer->tax();

        notifyMessage(currentPlayer->getName() + " will collect " + std::to_string(taxAmount) + " coins from tax.");
        notifyStateChange();

        // Don't end turn - wait for user
    } catch (const std::exception& e) {
        notifyError(e.what());
    }
}

void GameController::performBribe() {
    if (!currentPlayer) return;

    try {
        currentPlayer->bribe();
        if (currentPlayer->getLastAction() == ActionType::Bribe) {
            waitingForBribeDecision = false;
            notifyMessage(currentPlayer->getName() + " used bribe for a bonus action!");
            notifyStateChange();
        } else {
            // Bribe was blocked
            waitingForBribeDecision = false;
            notifyMessage(currentPlayer->getName() + "'s bribe was blocked!");
            endTurn();
        }
    } catch (const std::exception& e) {
        notifyError(e.what());
        waitingForBribeDecision = false;
    }
}

void GameController::performArrest(const std::string& targetName) {
    if (!currentPlayer || !turnInProgress) return;

    Player* target = getPlayer(targetName);
    if (!target) {
        notifyError("Target player not found!");
        return;
    }

    if (!game || !game->isAlive(*target)) {
        notifyError("Cannot arrest eliminated player!");
        return;
    }

    try {
        currentPlayer->arrest(*target);
        notifyMessage(currentPlayer->getName() + " will arrest " + targetName + ".");
        notifyStateChange();

        // Don't end turn - wait for user
    } catch (const std::exception& e) {
        notifyError(e.what());
    }
}

void GameController::performSanction(const std::string& targetName) {
    if (!currentPlayer || !turnInProgress) return;

    Player* target = getPlayer(targetName);
    if (!target) {
        notifyError("Target player not found!");
        return;
    }

    if (!game || !game->isAlive(*target)) {
        notifyError("Cannot sanction eliminated player!");
        return;
    }

    try {
        int cost = (target->getRoleName() == "Judge") ? 4 : 3;
        if (currentPlayer->getCoins() < cost) {
            notifyError("Not enough coins to sanction " + target->getRoleName() + " (need " + std::to_string(cost) + ")");
            return;
        }

        currentPlayer->sanction(*target);
        notifyMessage(currentPlayer->getName() + " will sanction " + targetName + " for " + std::to_string(cost) + " coins.");
        notifyStateChange();

        // Don't end turn - wait for user
    } catch (const std::exception& e) {
        notifyError(e.what());
    }
}

void GameController::performCoup(const std::string& targetName) {
    if (!currentPlayer) return;

    Player* target = getPlayer(targetName);
    if (!target) {
        notifyError("Target player not found!");
        return;
    }

    if (!game || !game->isAlive(*target)) {
        notifyError("Cannot coup eliminated player!");
        return;
    }

    try {
        // For forced coup with 10+ coins
        if (!turnInProgress && currentPlayer->getCoins() >= 10) {
            currentPlayer->resetTurnState();
            turnInProgress = true;
        }

        currentPlayer->coup(*target);

        // Check if coup was blocked
        if (game->isAlive(*target)) {
            notifyMessage(currentPlayer->getName() + "'s coup was blocked!");
        } else {
            notifyMessage(currentPlayer->getName() + " couped " + targetName + "!");
        }

        // Coup always ends turn
        turnInProgress = false;
        waitingForBribeDecision = false;
        updateCurrentPlayer();

        if (isGameOver()) {
            notifyMessage("Game Over! Winner: " + getWinner());
        } else if (currentPlayer) {
            notifyMessage("Now it's " + getCurrentPlayerName() + "'s turn.");
        }

        notifyStateChange();
    } catch (const std::exception& e) {
        notifyError(e.what());
        turnInProgress = false;
        waitingForBribeDecision = false;
    }
}

void GameController::performInvest() {
    if (!currentPlayer || !turnInProgress) return;

    Baron* baron = dynamic_cast<Baron*>(currentPlayer);
    if (!baron) {
        notifyError("Only Baron can invest!");
        return;
    }

    try {
        baron->invest();
        notifyMessage(currentPlayer->getName() + " invested (3 → 6 coins).");
        notifyStateChange();

        // Don't end turn - wait for user
    } catch (const std::exception& e) {
        notifyError(e.what());
    }
}

void GameController::performUndo(const std::string& targetName) {
    if (!currentPlayer) return;

    Governor* gov = dynamic_cast<Governor*>(currentPlayer);
    if (!gov) {
        notifyError("Only Governor can undo!");
        return;
    }

    Player* target = getPlayer(targetName);
    if (!target) {
        notifyError("Target player not found!");
        return;
    }

    try {
        gov->undo(*target);
        notifyMessage(currentPlayer->getName() + " undid " + targetName + "'s tax.");
        notifyStateChange();
    } catch (const std::exception& e) {
        notifyError(e.what());
    }
}

int GameController::peekCoins(const std::string& targetName) {
    if (!currentPlayer) return -1;

    Spy* spy = dynamic_cast<Spy*>(currentPlayer);
    if (!spy) {
        notifyError("Only Spy can peek!");
        return -1;
    }

    Player* target = getPlayer(targetName);
    if (!target) {
        notifyError("Target player not found!");
        return -1;
    }

    if (!game || !game->isAlive(*target)) {
        notifyError("Cannot peek at eliminated player!");
        return -1;
    }

    try {
        int coins = spy->peekCoins(*target);
        notifyMessage(targetName + " has " + std::to_string(coins) + " coins.");
        return coins;
    } catch (const std::exception& e) {
        notifyError(e.what());
        return -1;
    }
}

void GameController::blockNextArrest(const std::string& targetName) {
    if (!currentPlayer) return;

    Spy* spy = dynamic_cast<Spy*>(currentPlayer);
    if (!spy) {
        notifyError("Only Spy can block arrests!");
        return;
    }

    Player* target = getPlayer(targetName);
    if (!target) {
        notifyError("Target player not found!");
        return;
    }

    if (!game || !game->isAlive(*target)) {
        notifyError("Cannot block arrest for eliminated player!");
        return;
    }

    try {
        spy->blockNextArrest(*target);
        notifyMessage("Blocked " + targetName + "'s next arrest.");
        notifyStateChange();
    } catch (const std::exception& e) {
        notifyError(e.what());
    }
}

void GameController::handleBribeDecision(bool useBribe) {
    if (!waitingForBribeDecision || !currentPlayer) return;

    if (useBribe) {
        performBribe();
    } else {
        waitingForBribeDecision = false;
        endTurn();
    }
}

bool GameController::canPerformAction(const std::string& action) const {
    if (!currentPlayer || !turnInProgress) return false;

    // Check specific conditions for each action
    if (action == "coup") {
        return currentPlayer->getCoins() >= 7;
    } else if (action == "bribe") {
        return currentPlayer->canUseBribe();
    } else if (action == "invest") {
        return currentPlayer->getRoleName() == "Baron" && currentPlayer->getCoins() >= 3;
    } else if (action == "sanction") {
        // Need to check target-specific cost, so return true if we have at least 3
        return currentPlayer->getCoins() >= 3;
    }

    return true; // Most actions don't have specific requirements
}

void GameController::notifyMessage(const std::string& msg) {
    std::cout << "[INFO] " << msg << std::endl;
    if (onMessage) onMessage(msg);
}

void GameController::notifyError(const std::string& msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
    if (onError) onError(msg);
}

void GameController::notifyStateChange() {
    if (onGameStateChanged) onGameStateChanged();
}

} // namespace gui
} // namespace coup