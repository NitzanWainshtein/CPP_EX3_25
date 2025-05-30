##Email: nitzanwa@gmail.com
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Include directories
INCLUDES = -I. -IGameLogic -IPlayers -IPlayers/Roles -ITests

# Source files
GAMELOGIC_SRCS = GameLogic/BankManager.cpp \
                 GameLogic/Game.cpp \
                 GameLogic/Logger.cpp \
                 GameLogic/PlayerFactory.cpp

PLAYERS_SRCS = Players/Player.cpp

ROLES_SRCS = Players/Roles/Baron.cpp \
             Players/Roles/General.cpp \
             Players/Roles/Governor.cpp \
             Players/Roles/Judge.cpp \
             Players/Roles/Merchant.cpp \
             Players/Roles/Spy.cpp

GUI_SRCS = GUI/GUI.cpp GUI/main_gui.cpp

TEST_SRCS = Tests/demo_test.cpp

# Combined source files
LIB_SRCS = $(GAMELOGIC_SRCS) $(PLAYERS_SRCS) $(ROLES_SRCS)
MAIN_SRCS = main.cpp $(LIB_SRCS)

# Object files
MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)
LIB_OBJS = $(LIB_SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
GUI_OBJS = $(GUI_SRCS:.cpp=.o)

# Target executables
MAIN_TARGET = coup_demo
TEST_TARGET = Tests/test_runner
GUI_TARGET = gui_app

# Default target
all: $(MAIN_TARGET)

# Main demo target - build and run
Main: $(MAIN_TARGET)
	./$(MAIN_TARGET)

# Build main executable
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Test target - build and run
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Build test executable
$(TEST_TARGET): $(TEST_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# GUI target - build GUI
gui: $(GUI_TARGET)

# Run GUI
run-gui: $(GUI_TARGET)
	./$(GUI_TARGET)

# Build GUI executable
$(GUI_TARGET): $(GUI_OBJS) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lsfml-graphics -lsfml-window -lsfml-system

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Memory check with valgrind
valgrind: $(MAIN_TARGET)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(MAIN_TARGET)

# Memory check on tests
test-valgrind: $(TEST_TARGET)
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(TEST_TARGET)

# Clean up
clean:
	rm -f $(MAIN_OBJS) $(TEST_OBJS) $(GUI_OBJS)
	rm -f $(MAIN_TARGET) $(TEST_TARGET) $(GUI_TARGET)

.PHONY: all Main test gui run-gui valgrind test-valgrind clean