#pragma once

#include <string>
#include <vector>
#include "Player.hpp"

namespace coup {

    class Player; // forward declaration

    class Game {
    private:
        std::vector<Player*> player_list;
        int current_turn_index;
        int bank;

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

        // NEW METHODS
        bool isGameOver() const;
        std::string winner() const;
    };

} // namespace coup