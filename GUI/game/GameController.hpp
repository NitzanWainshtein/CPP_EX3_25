// GUI/game/GameController.hpp
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "../../GameLogic/Game.hpp"
#include "../../Players/Player.hpp"

namespace coup {
namespace gui {

// This class is the bridge between GUI and the actual game logic
class GameController {
private:
    std::unique_ptr<Game> game;
    std::vector<Player*> players;

    // Current state
    bool turnInProgress = false;
    bool waitingForBribeDecision = false;
    Player* currentPlayer = nullptr;

    // Callbacks for GUI updates
    std::function<void(const std::string&)> onMessage;
    std::function<void(const std::string&)> onError;
    std::function<void()> onGameStateChanged;

public:
    GameController() = default;
    ~GameController();

    // Game setup
    bool createGame(const std::vector<std::string>& playerNames);
    void resetGame();

    // Game state queries
    bool isGameActive() const { return game != nullptr; }
    bool isGameOver() const;
    std::string getCurrentPlayerName() const;
    std::string getWinner() const;
    int getBankCoins() const;

    // Player queries
    std::vector<std::string> getPlayerNames() const;
    Player* getPlayer(const std::string& name) const;
    Player* getCurrentPlayer() const { return currentPlayer; }
    bool isPlayerAlive(const std::string& name) const;

    // Turn management
    bool canStartTurn() const;
    void startTurn();
    void endTurn();
    bool isTurnInProgress() const { return turnInProgress; }
    bool isWaitingForBribe() const { return waitingForBribeDecision; }

    // Actions
    bool canPerformAction(const std::string& action) const;
    void performGather();
    void performTax();
    void performBribe();
    void performArrest(const std::string& targetName);
    void performSanction(const std::string& targetName);
    void performCoup(const std::string& targetName);

    // Special abilities
    void performInvest();  // Baron
    void performUndo(const std::string& targetName);  // Governor
    int peekCoins(const std::string& targetName);  // Spy
    void blockNextArrest(const std::string& targetName);  // Spy

    // Bribe decision
    void handleBribeDecision(bool useBribe);

    // Callbacks
    void setMessageCallback(std::function<void(const std::string&)> cb) { onMessage = cb; }
    void setErrorCallback(std::function<void(const std::string&)> cb) { onError = cb; }
    void setStateChangeCallback(std::function<void()> cb) { onGameStateChanged = cb; }

private:
    void setupPlayerCallbacks();
    void updateCurrentPlayer();
    void notifyMessage(const std::string& msg);
    void notifyError(const std::string& msg);
    void notifyStateChange();
};

} // namespace gui
} // namespace coup