# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -IPlayers -IGameLogic


# Directories
SRC_DIR = .
GAME_DIR = GameLogic
PLAYER_DIR = Players
ROLES_DIR = $(PLAYER_DIR)/Roles

# Files
MAIN = main.cpp
OBJS = \
	$(GAME_DIR)/Game.cpp \
	$(PLAYER_DIR)/Player.cpp \
	$(ROLES_DIR)/Governor.cpp

# Output
EXEC = main

# Targets
all: $(EXEC)

$(EXEC): $(MAIN) $(OBJS)
	$(CXX) $(CXXFLAGS) $(MAIN) $(OBJS) -o $(EXEC)

run: $(EXEC)
	./$(EXEC)

valgrind: $(EXEC)
	valgrind ./$(EXEC)

clean:
	rm -f $(EXEC)

.PHONY: all clean run valgrind
