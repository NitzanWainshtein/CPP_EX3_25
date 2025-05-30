// Email: nitzanwa@gmail.com

#ifndef ACTION_TYPE_HPP
#define ACTION_TYPE_HPP

namespace coup {

    /**
     * @enum ActionType
     * @brief Represents all possible actions in the Coup game
     */
    enum class ActionType {
        None,       ///< No action (default state)
        Gather,     ///< Take 1 coin from the bank
        Tax,        ///< Collect tax (2-3 coins depending on role)
        Bribe,      ///< Pay 4 coins for extra action
        Arrest,     ///< Steal 1 coin from another player
        Sanction,   ///< Block economic actions for 1 turn
        Coup,       ///< Eliminate another player for 7 coins
        Invest      ///< Baron special: pay 3 to get 6
    };

}

#endif // ACTION_TYPE_HPP