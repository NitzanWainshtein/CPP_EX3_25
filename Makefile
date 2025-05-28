# Email: nitzanwa@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
INCLUDES = -IGameLogic -IPlayers -IPlayers/Roles -IGUI

# SFML libraries for GUI
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Source directories
SRC_DIRS = GameLogic Players Players/Roles GUI

# ===== Core Game Sources (shared by all targets) =====
GAMELOGIC_SRC = $(wildcard GameLogic/*.cpp)
PLAYERS_SRC = $(wildcard Players/*.cpp)
ROLES_SRC = $(wildcard Players/Roles/*.cpp)
CORE_OBJ = $(GAMELOGIC_SRC:.cpp=.o) $(PLAYERS_SRC:.cpp=.o) $(ROLES_SRC:.cpp=.o)

# ===== Old GUI Sources (for backward compatibility) =====
OLD_GUI_SRC = GUI/ConsoleGUI.cpp GUI/GameGUI.cpp
OLD_GUI_OBJ = $(OLD_GUI_SRC:.cpp=.o)

# ===== New Modular GUI Sources =====
GUI_CORE_SRC = GUI/core/ResourceManager.cpp GUI/core/SceneManager.cpp
GUI_COMPONENTS_SRC = GUI/components/Button.cpp GUI/components/Popup.cpp
GUI_SCENES_SRC = GUI/scenes/MainMenuScene.cpp GUI/scenes/PlayerCountScene.cpp \
                 GUI/scenes/PlayerNamesScene.cpp GUI/scenes/GameScene.cpp \
                 GUI/scenes/GameOverScene.cpp
GUI_GAME_SRC = GUI/game/GameController.cpp
NEW_GUI_SRC = $(GUI_CORE_SRC) $(GUI_COMPONENTS_SRC) $(GUI_SCENES_SRC) $(GUI_GAME_SRC) GUI/CoupGUI.cpp
NEW_GUI_OBJ = $(NEW_GUI_SRC:.cpp=.o)

# ===== Executable Names =====
DEMO_BIN = demo
TEST_BIN = test
OLD_GUI_BIN = gui_old
NEW_GUI_BIN = gui
CONSOLE_BIN = console

# ===== Default Target - Build the new GUI =====
all: $(NEW_GUI_BIN)

# ===== Main Targets =====

# Demo executable (using main.cpp)
$(DEMO_BIN): main.o $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Test executable (if test.cpp exists)
$(TEST_BIN): test.o $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Old GUI (for backward compatibility) - uses GUI/main_gui.cpp with old GUI files
$(OLD_GUI_BIN): GUI/main_gui_old.o $(OLD_GUI_OBJ) $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

# New modular GUI (main target) - uses GUI/main_gui.cpp
$(NEW_GUI_BIN): GUI/main_gui.o $(NEW_GUI_OBJ) $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

# Console version (if exists)
$(CONSOLE_BIN): GUI/main_console.o GUI/ConsoleGUI.o $(CORE_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ===== Object File Rules =====

# General rule for .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Special rules for main files
GUI/main_gui.o: GUI/main_gui.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

GUI/main_console.o: GUI/main_console.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test.o: Test/Test.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ===== Convenience Targets =====
Main: $(DEMO_BIN)
Demo: $(DEMO_BIN)
demo: $(DEMO_BIN)

GUI: $(NEW_GUI_BIN)
gui_target: $(NEW_GUI_BIN)

OldGUI: $(OLD_GUI_BIN)
old-gui: $(OLD_GUI_BIN)

Console: $(CONSOLE_BIN)
console: $(CONSOLE_BIN)

Test: $(TEST_BIN)
test: $(TEST_BIN)

# ===== Development Tools =====

# Memory leak check with valgrind
valgrind: $(NEW_GUI_BIN)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(NEW_GUI_BIN)

valgrind-demo: $(DEMO_BIN)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(DEMO_BIN)

valgrind-console: $(CONSOLE_BIN)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(CONSOLE_BIN)

# Run targets
run: $(NEW_GUI_BIN)
	./$(NEW_GUI_BIN)

run-demo: $(DEMO_BIN)
	./$(DEMO_BIN)

run-console: $(CONSOLE_BIN)
	./$(CONSOLE_BIN)

# ===== Clean Targets =====

# Clean all build artifacts
clean:
	rm -f $(DEMO_BIN) $(TEST_BIN) $(OLD_GUI_BIN) $(NEW_GUI_BIN) $(CONSOLE_BIN)
	rm -f *.o
	rm -f $(CORE_OBJ) $(OLD_GUI_OBJ) $(NEW_GUI_OBJ)
	rm -f GUI/main_gui.o GUI/main_console.o GUI/main_gui_old.o

# Clean only object files
clean-obj:
	rm -f *.o
	rm -f $(CORE_OBJ) $(OLD_GUI_OBJ) $(NEW_GUI_OBJ)
	rm -f GUI/main_gui.o GUI/main_console.o

# Clean only GUI files
clean-gui:
	rm -f $(NEW_GUI_BIN) $(OLD_GUI_BIN)
	rm -f $(NEW_GUI_OBJ) $(OLD_GUI_OBJ)
	rm -f GUI/main_gui.o GUI/main_gui_old.o

# Force rebuild
rebuild: clean all

rebuild-gui: clean-gui gui

# ===== Setup and Dependencies =====

# Setup directory structure
setup-dirs:
	mkdir -p GUI/core GUI/components GUI/scenes GUI/game GUI/images
	@echo "✅ Directory structure created"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@command -v g++ >/dev/null 2>&1 || { echo "❌ g++ not found"; exit 1; }
	@pkg-config --exists sfml-all && echo "✅ SFML found" || echo "❌ SFML not found - install with: sudo apt-get install libsfml-dev"
	@echo "✅ All checks completed"

# Install SFML (Ubuntu/Debian)
install-sfml:
	sudo apt-get update
	sudo apt-get install -y libsfml-dev

# ===== Help =====
help:
	@echo "Coup Game - Makefile Help"
	@echo "========================="
	@echo ""
	@echo "Main targets:"
	@echo "  make / make gui    - Build the new modular GUI (default)"
	@echo "  make demo          - Build text demo executable"
	@echo "  make test          - Build test executable"
	@echo "  make console       - Build console GUI version"
	@echo "  make old-gui       - Build old GUI (backward compatibility)"
	@echo ""
	@echo "Run targets:"
	@echo "  make run           - Build and run new GUI"
	@echo "  make run-demo      - Build and run demo"
	@echo "  make run-console   - Build and run console"
	@echo ""
	@echo "Development:"
	@echo "  make valgrind      - Run new GUI with memory check"
	@echo "  make valgrind-demo - Run demo with memory check"
	@echo "  make rebuild       - Clean and rebuild everything"
	@echo "  make rebuild-gui   - Clean and rebuild GUI only"
	@echo ""
	@echo "Setup:"
	@echo "  make setup-dirs    - Create directory structure"
	@echo "  make check-deps    - Check if dependencies are installed"
	@echo "  make install-sfml  - Install SFML (Ubuntu/Debian)"
	@echo ""
	@echo "Cleaning:"
	@echo "  make clean         - Remove all build artifacts"
	@echo "  make clean-obj     - Remove only object files"
	@echo "  make clean-gui     - Remove GUI-related files"
	@echo ""
	@echo "Files structure:"
	@echo "  GUI/main_gui.cpp   - Entry point for new GUI"
	@echo "  main.cpp           - Entry point for demo"
	@echo "  GUI/main_console.cpp - Entry point for console GUI"

# ===== Debug Information =====
show-sources:
	@echo "=== Core Game Sources ==="
	@echo "GameLogic: $(GAMELOGIC_SRC)"
	@echo "Players: $(PLAYERS_SRC)"
	@echo "Roles: $(ROLES_SRC)"
	@echo ""
	@echo "=== New GUI Sources ==="
	@echo "Core: $(GUI_CORE_SRC)"
	@echo "Components: $(GUI_COMPONENTS_SRC)"
	@echo "Scenes: $(GUI_SCENES_SRC)"
	@echo "Game: $(GUI_GAME_SRC)"
	@echo ""
	@echo "=== Object Files ==="
	@echo "Core objects: $(CORE_OBJ)"
	@echo "New GUI objects: $(NEW_GUI_OBJ)"

# Count lines of code
count:
	@echo "Lines of code:"
	@echo -n "GameLogic: "; find GameLogic -name "*.cpp" -o -name "*.hpp" | xargs wc -l | tail -1
	@echo -n "Players: "; find Players -name "*.cpp" -o -name "*.hpp" | xargs wc -l | tail -1
	@echo -n "GUI: "; find GUI -name "*.cpp" -o -name "*.hpp" | xargs wc -l | tail -1
	@echo -n "Total: "; find . -name "*.cpp" -o -name "*.hpp" | xargs wc -l | tail -1

# ===== Phony Targets =====
.PHONY: all clean clean-obj clean-gui rebuild rebuild-gui help check-deps install-sfml setup-dirs
.PHONY: Main Demo demo GUI gui OldGUI old-gui Console console Test test
.PHONY: valgrind valgrind-demo valgrind-console run run-demo run-console
.PHONY: show-sources count