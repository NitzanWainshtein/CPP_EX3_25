# Coup Game – CPP_EX3

**Author:** Nitzan Wainshtein
**ID:** 209086263
**Email:** [nitzanwa@gmail.com](mailto:nitzanwa@gmail.com)
**Repository:** [https://github.com/NitzanWainshtein/CPP_EX3_25](https://github.com/NitzanWainshtein/CPP_EX3_25)

## Table of Contents

* Project Overview
* Game Description
* Project Structure
* Features Implemented
* Requirements
* Installation & Setup
* How to Run
* Game Rules
* Character Roles
* Technical Implementation
* Testing
* Memory Management

## Project Overview

This project implements the Coup card game in C++ as part of Assignment 3.
It is a strategic game for 2–6 players, where players assume roles and compete to be the last standing using coins, abilities, and blocking strategies.

## Game Description

Players:

* Start with a unique role.
* Take turns performing actions to gain coins and eliminate others.
* Use role abilities and block opponent actions.
* Aim to be the last survivor.

## Project Structure

```
CPP_EX3_25/
├── main.cpp
├── Makefile
├── README.md
│
├── GameLogic/
│   ├── Game.hpp/.cpp
│   ├── BankManager.hpp/.cpp
│   ├── Logger.hpp/.cpp
│   └── PlayerFactory.hpp/.cpp
│
├── Players/
│   ├── Player.hpp/.cpp
│   └── Roles/
│       ├── Governor.hpp/.cpp
│       ├── Judge.hpp/.cpp
│       ├── General.hpp/.cpp
│       ├── Baron.hpp/.cpp
│       ├── Merchant.hpp/.cpp
│       └── Spy.hpp/.cpp
│
├── Tests/
│   └── demo_test.cpp
│
└── GUI/
    ├── GUI.hpp/.cpp
    └── main_gui.cpp
```

## Features Implemented

* Turn-based gameplay.
* Bank system with 200 coins.
* Player elimination and win detection.
* Actions: gather, tax, bribe, arrest, sanction, coup.
* Six unique roles with special abilities.
* Blocking mechanics and status effects.
* Full logging system.
* Valgrind-verified memory safety.

## Requirements

* g++ with C++17.
* Linux (tested on Ubuntu).
* Valgrind (optional, for memory checks).
* SFML (optional, for GUI).

## Installation & Setup

```bash
git clone https://github.com/NitzanWainshtein/CPP_EX3_25.git
cd CPP_EX3_25
```

Check:

```bash
g++ --version
make --version
```

### Installing SFML (for GUI mode)

To run the graphical version, install the SFML library:

```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```

If using another OS (e.g., Windows or macOS), refer to [https://www.sfml-dev.org/download.php](https://www.sfml-dev.org/download.php) for platform-specific installation instructions.

## How to Run

Build:

```bash
make Main
```

Run tests:

```bash
make test
```

Check memory:

```bash
make valgrind
```

Run GUI:

```bash
make gui
make run-gui
```

## Game Rules

* Gather: +1 coin.
* Tax: +2 coins (Governor +3).
* Bribe: Pay 4 coins for extra action.
* Arrest: Take 1 coin from opponent.
* Sanction: Block economic actions.
* Coup: Eliminate for 7 coins (mandatory at 10+).

Win: last player standing.

## Character Roles

* Governor: +3 tax, block tax.
* Judge: Block bribes, harder to sanction.
* General: Block coup, bonus on arrest.
* Baron: Invest coins, bonus on sanction.
* Merchant: Bonus if 3+ coins, bonus on arrest.
* Spy: Peek coins, block arrests.

## Testing

* 30+ test cases.
* Edge cases, integration, role-specific tests.
* Run:

```bash
make test
make test-valgrind
```

## Memory Management

* Verified with Valgrind.
* Exception-safe.
* Automatic cleanup in destructors.
