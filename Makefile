# Email: nitzanwa@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
INCLUDES = -IGameLogic -IPlayers -IPlayers/Roles

# Source directories
SRC_DIRS = GameLogic Players Players/Roles

# Find all .cpp files in the source directories
GAMELOGIC_SRC = $(wildcard GameLogic/*.cpp)
PLAYERS_SRC = $(wildcard Players/*.cpp)
ROLES_SRC = $(wildcard Players/Roles/*.cpp)

# All source files except main.cpp and test.cpp
LIB_SRC = $(GAMELOGIC_SRC) $(PLAYERS_SRC) $(ROLES_SRC)

# Object files for the library
LIB_OBJ = $(LIB_SRC:.cpp=.o)

# Executable names
MAIN_BIN = main
TEST_BIN = test

# Default target
all: $(MAIN_BIN)

# Main executable
$(MAIN_BIN): main.o $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Main object file
main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp

# Test executable
$(TEST_BIN): test.o $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Test object file
test.o: test.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c test.cpp

# Rule for creating object files from cpp files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Alternative targets for compatibility
Main: $(MAIN_BIN)

Test: $(TEST_BIN)

test: $(TEST_BIN)

# Memory leak check with valgrind
valgrind: $(MAIN_BIN)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(MAIN_BIN)

# Clean build artifacts
clean:
	rm -f $(MAIN_BIN) $(TEST_BIN) *.o $(LIB_OBJ)

# Clean only object files
clean-obj:
	rm -f *.o $(LIB_OBJ)

# Force rebuild
rebuild: clean all

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build main executable (default)"
	@echo "  main       - Build main executable"
	@echo "  Main       - Build main executable (alternative)"
	@echo "  test       - Build test executable"
	@echo "  Test       - Build test executable (alternative)"
	@echo "  valgrind   - Run main with valgrind memory check"
	@echo "  clean      - Remove all build artifacts"
	@echo "  clean-obj  - Remove only object files"
	@echo "  rebuild    - Clean and rebuild everything"
	@echo "  help       - Show this help message"

# Show source files (for debugging makefile)
show-sources:
	@echo "GameLogic sources: $(GAMELOGIC_SRC)"
	@echo "Players sources: $(PLAYERS_SRC)"
	@echo "Roles sources: $(ROLES_SRC)"
	@echo "Library objects: $(LIB_OBJ)"

# Phony targets
.PHONY: all Main Test test clean clean-obj rebuild help show-sources valgrind