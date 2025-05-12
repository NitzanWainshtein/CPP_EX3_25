# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Include paths
INCLUDES = -IPlayers -IPlayers/Roles -IGameLogic

# Source files
SRC = main.cpp \
      $(wildcard GameLogic/*.cpp) \
      $(wildcard Players/*.cpp) \
      $(wildcard Players/Roles/*.cpp)

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable name
TARGET = sim

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile rule
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
