#pragma once

#include <string>
#include <vector>
#include "../Players/Player.hpp"

namespace coup {

    class Player;

    enum class ActionType {
        None,
        Tax,
        Bribe,
        Coup,
        Sanction,
        Arrest,
        Gather,
        Invest
    };

    class Game {
    private:
        std::vector<Player*> player_list; //Players list
        int current_turn_index; //Who's turn it is
        int bank; //Game bank

        // Pending action tracking
        Player* lastActingPlayer;
        Player* lastActionTarget;
        ActionType lastActionType;
        bool actionPending;

    public:
        Game();

        bool nameExists(const std::string& name) const;
        void addPlayer(Player* player);
        std::vector<std::string> players() const;
        std::string turn() const;
        void nextTurn();
        void eliminate(Player& player);
        void resetPlayers();
        int getBank() const;
        void addToBank(int amount);
        void takeFromBank(int amount);

        bool isGameOver() const;
        bool isAlive(const Player& player) const;
        std::string winner() const;

        // Pending action logic
        void setPendingAction(Player* actor, ActionType action, Player* target = nullptr);
        void resolvePendingAction();
        bool hasPendingAction() const;
        Player* getLastActor() const;
        ActionType getLastActionType() const;
        Player* getLastTarget() const;

        void requestImmediateResponse(Player *actor, ActionType action, Player *target);
    };

}