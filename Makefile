CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Include paths
INCLUDES = -IPlayers -IPlayers/Roles -IGameLogic

# Source files (excluding GUI)
SRC = main.cpp \
      GameLogic/Game.cpp \
      GameLogic/BankManager.cpp \
      GameLogic/PlayerFactory.cpp \
      Players/Player.cpp \
      Players/Roles/Governor.cpp \
      Players/Roles/Baron.cpp \
      Players/Roles/Spy.cpp \
      Players/Roles/Merchant.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Final binary
TARGET = sim

# Default build target
all: $(TARGET)

# Linking final binary
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compiling object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run executable
run: $(TARGET)
	./$(TARGET)

# Run with Valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET)
