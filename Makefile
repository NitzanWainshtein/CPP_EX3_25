# Email: nitzanwa@gmail.com

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g
INCLUDES = -IGUI -IGameLogic -IPlayers -IPlayers/Roles
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIRS = GUI GameLogic Players Players/Roles
SRC = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJ = $(SRC:.cpp=.o)

BIN = main
TEST = test

all: $(BIN)

$(BIN): $(OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp

test: $(OBJ) test.o
	$(CXX) $(CXXFLAGS) -o $(TEST) $^ $(LIBS)

test.o: test.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c test.cpp

valgrind: $(BIN)
	valgrind --leak-check=full --track-origins=yes ./$(BIN)

clean:
	rm -f $(BIN) $(TEST) *.o */*.o */*/*.o
