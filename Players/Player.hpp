#pragma once

#include <string>

namespace coup {
    class Game;               // forward declaration
    enum class ActionType;    // forward declaration

    class Player {
    protected:
        Game &game;
        std::string name;
        int coins;
        bool sanctioned;
        bool arrested;

        ActionType lastAction;
        Player *lastActionTarget;
        bool actionBlocked;
        bool bribeUsedThisTurn;
        bool arrestBlocked;

        void requireTurn() const;
        void requireAlive(const Player &target) const;

    public:
        Player(Game &game, const std::string &name);
        virtual ~Player();

        std::string getName() const;
        int getCoins() const;
        bool isSanctioned() const;
        ActionType getLastAction() const;

        void blockLastAction();
        void blockArrestNextTurn();
        void setCoins(int coins);

        virtual void startTurn();

        // Actions
        void gather();
        virtual void tax();
        virtual int taxAmount() const;
        virtual void bribe();
        virtual void arrest(Player &player);
        virtual void sanction(Player &player);
        virtual void coup(Player &player);

        virtual void undo(Player &player);

        virtual std::string getRoleName() const = 0;

        void endTurn();
    };
}
