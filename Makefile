# Email: nitzanwa@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
INCLUDES = -IGameLogic -IPlayers -IPlayers/Roles -IGUI

# SFML libraries for GUI
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Source directories
SRC_DIRS = GameLogic Players Players/Roles GUI

# Find all .cpp files in the source directories (excluding GUI/main_console.cpp to avoid conflicts)
GAMELOGIC_SRC = $(wildcard GameLogic/*.cpp)
PLAYERS_SRC = $(wildcard Players/*.cpp)
ROLES_SRC = $(wildcard Players/Roles/*.cpp)
GUI_SRC = $(filter-out GUI/main_console.cpp, $(wildcard GUI/*.cpp))

# All source files except main files
LIB_SRC = $(GAMELOGIC_SRC) $(PLAYERS_SRC) $(ROLES_SRC) $(GUI_SRC)

# Object files for the library
LIB_OBJ = $(LIB_SRC:.cpp=.o)

# Executable names
MAIN_BIN = main
TEST_BIN = test
GUI_BIN = gui
CONSOLE_BIN = console

# Default target - now builds GUI version
all: $(GUI_BIN)

# GUI executable (main target) - uses GUI/main_gui.cpp
$(GUI_BIN): GUI/main_gui.o $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

# Console executable - uses GUI/main_console.cpp
$(CONSOLE_BIN): GUI/main_console.o GameLogic/Game.o GameLogic/BankManager.o Players/Player.o $(ROLES_SRC:.cpp=.o) GameLogic/PlayerFactory.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Main executable (demo) - uses main.cpp
$(MAIN_BIN): main.o $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Test executable
$(TEST_BIN): test.o $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# GUI Main object file
GUI/main_gui.o: GUI/main_gui.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c GUI/main_gui.cpp -o GUI/main_gui.o

# Console main object file
GUI/main_console.o: GUI/main_console.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c GUI/main_console.cpp -o GUI/main_console.o

# Main object file
main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp

# Test object file
test.o: test.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c test.cpp

# Rule for creating object files from cpp files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Alternative targets for compatibility
Main: $(MAIN_BIN)
Demo: $(MAIN_BIN)
GUI: $(GUI_BIN)
gui: $(GUI_BIN)
Console: $(CONSOLE_BIN)
console: $(CONSOLE_BIN)

Test: $(TEST_BIN)
test: $(TEST_BIN)

# Memory leak check with valgrind
valgrind: $(GUI_BIN)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(GUI_BIN)

valgrind-demo: $(MAIN_BIN)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(MAIN_BIN)

valgrind-console: $(CONSOLE_BIN)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(CONSOLE_BIN)

# Clean build artifacts
clean:
	rm -f $(MAIN_BIN) $(TEST_BIN) $(GUI_BIN) $(CONSOLE_BIN) *.o GUI/main_gui.o GUI/main_console.o $(LIB_OBJ)

# Clean only object files
clean-obj:
	rm -f *.o GUI/main_gui.o GUI/main_console.o $(LIB_OBJ)

# Force rebuild
rebuild: clean all

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build GUI executable (default)"
	@echo "  gui        - Build GUI executable"
	@echo "  console    - Build console executable"
	@echo "  main       - Build main demo executable"
	@echo "  Demo       - Build main demo executable"
	@echo "  test       - Build test executable"
	@echo "  valgrind   - Run GUI with valgrind memory check"
	@echo "  valgrind-demo - Run main demo with valgrind memory check"
	@echo "  valgrind-console - Run console with valgrind memory check"
	@echo "  clean      - Remove all build artifacts"
	@echo "  clean-obj  - Remove only object files"
	@echo "  rebuild    - Clean and rebuild everything"
	@echo "  help       - Show this help message"

# Show source files (for debugging makefile)
show-sources:
	@echo "GameLogic sources: $(GAMELOGIC_SRC)"
	@echo "Players sources: $(PLAYERS_SRC)"
	@echo "Roles sources: $(ROLES_SRC)"
	@echo "GUI sources: $(GUI_SRC)"
	@echo "Library objects: $(LIB_OBJ)"

# Check SFML installation
check-sfml:
	@echo "Checking SFML installation..."
	@pkg-config --exists sfml-all && echo "✅ SFML found" || echo "❌ SFML not found - install with: sudo apt-get install libsfml-dev"

# Phony targets
.PHONY: all Main Demo GUI gui Console console Test test clean clean-obj rebuild help show-sources valgrind valgrind-demo valgrind-console check-sfml